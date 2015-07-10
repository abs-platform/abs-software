package abs.com.test.appmodule.services;

import android.app.Service;
import android.content.Intent;
import android.os.*;
import android.util.Log;
import android.widget.Toast;

import com.abs.payloadsdk.*;
import com.abs.payloadsdk.arduino.*;

import java.io.File;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import abs.com.test.appmodule.classes.Mathematics;

/**
 * Main class for the Tests' Service. It contains all the functions to perform all the tests.
 * Selecting a test is easy and it is done via a test ID and a switch-case on the runTest()
 * function.
 */
public class TestService extends PayloadApp
{
    public static final String TAG = "TestService";

    private final IBinder mBinder = new LocalBinder();
    private Arduino arduino;

    /* This class has to be defined here because if it wasn't we couldn't return this instance of
     TestService */
    public class LocalBinder extends Binder
    {
        public TestService getService()
        {
            /* Return this instance of LocalService so clients can call public methods */
            return TestService.this;
        }
    }

    public TestService() {
        super("test-service");
    }

    @Override
    public void onCreate()
    {
        this.arduino = this.getArduino();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        Toast.makeText(this, "Starting service...", Toast.LENGTH_SHORT).show();

        return Service.START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        Toast.makeText(getApplicationContext(), "Binding...", Toast.LENGTH_SHORT).show();

        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent)
    {
        /* Return true to allow onRebind */
        return false;
    }

    @Override
    public void onRebind(Intent intent)
    {
        super.onRebind(intent);
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
    }

    /**
     * Does the test selection evaluating the given ID
     * @param id The test ID
     */
    public List<Double> runTest(String id)
    {
        List<Double> result = new ArrayList<>();

        switch(id) {
            case "latency":
                /* Get errors the results */
                Map<Integer, Integer> errors = latencyErrorsTest(4, 500);

                writeLog(id + "-errors", errors.toString());
                /* Add errors to results list to pass them to the activity */
                for(Map.Entry<Integer, Integer> error : errors.entrySet())
                    result.add(error.getValue().doubleValue());

                break;

            case "events":
                Toast.makeText(this, "Starting event collision test...",
                        Toast.LENGTH_SHORT).show();

                eventsTest(id);

                break;

            case "services":
                Toast.makeText(this, "Starting multiple services test...",
                        Toast.LENGTH_SHORT).show();

                getLatencies(4, 1, 500);

                break;

            default:
                throw new RuntimeException("Incorrect test ID");
        }

        stopSelf();

        return result;
    }

    /**
     * Performs the latencies test for various intervals and getting the latencies and error count
     * for each interval
     * @param pin The pin to which it writes
     * @param times The number of times the latency is measured
     * @return A Map containing how many errors have occurred in each interval
     */
    private Map<Integer, Integer> latencyErrorsTest(int pin, int times)
    {
        Map<Integer, Integer> errors = new HashMap<>();

        int x, errorCount;
        for(x=0; x<=4; x++) {
            /* Update interval making it sweep from 0 to 200ms in 50ms jumps */
            int interval = 50*x;

            errorCount = getLatencies(pin, interval, times);

            /* Update the errors map with the actual test info */
            errors.put(interval, errorCount);
        }

        return errors;
    }

    /**
     * Gets the acual latencies
     * @param pin The pin to which it writes
     * @param interval The interval between samples
     * @param times The times the latency is measured
     * @return The number of errors that have occurred
     */
    private int getLatencies(int pin, int interval, int times)
    {
        List<Double> latencies = new ArrayList<>();

        /* Clean error count variable for the new test with the new latency */
        int y, errorCount = 0;

        for(y=0; y<times; y++) {
            /* Get the start time */
            double start = System.nanoTime();

            try {
                if(y%2 == 0) {
                    if(this.arduino.digitalWrite(pin, 1) == 0) {
                        /* Add the measured latency to the latencies list */
                        Double latency = this.getLatency(start);
                        latencies.add(latency);
                    }
                    else {
                        errorCount++;
                        Log.d(TAG, "Error!!");
                    }
                } else {
                    if(this.arduino.digitalWrite(pin, 0) == 0) {
                        /* Add the measured latency to the latencies list */
                        Double latency = this.getLatency(start);
                        latencies.add(latency);
                    }
                    else {
                        errorCount++;
                        Log.d(TAG, "Error!!");
                    }
                }
            } catch(Exception e) {
                e.printStackTrace();
            }

            /* Wait the specified interval between digitalWrites */
            waitMs(interval);
        }

        writeResults("latency-" + interval, latencies);

        return errorCount;
    }

    /**
     * Event collision test.
     * It launches 2 events with the same frequency and waits 10000ms. Then it calls dumpBuffer
     * at the same time for both events. Evaluates if there is a collision.
     * @param id the test id
     */
    private void eventsTest(String id)
    {
        /* Event 1 */
        EventHandler ev1 = this.arduino.createEvent(2, Event.ACTIONS.ANALOG_READ, new byte[]{4, 0});

        /* Event 2 */
        EventHandler ev2 = this.arduino.createEvent(2, Event.ACTIONS.ANALOG_READ, new byte[]{4, 0});

        /* Wait a certain time to allow the events to gather some data */
        waitMs(20000);

        /* Dump the event1 buffer data */
        Log.d(TAG, "Dumping buffer 1...");
        ArrayList<Byte> data1 = this.dumpBuffer(ev1);

        if(data1.size() > 0) {
            ev1.stopEvent();
        }

        Log.d(TAG, "Dumping buffer 2...");
        /* Dump the event2 buffer data */
        ArrayList<Byte> data2 = this.dumpBuffer(ev2);

        if(data2.size() > 0) {
            ev2.stopEvent();
        }

        writeLog(id, data1.toString() + "\n" + data2.toString());
    }

    /**
     * Starts the number of services that we specify
     * @param maxServ Number of services started
     */
    private void servicesTest(int maxServ)
    {
        for(int c=0; c<maxServ; c++)
        {
            this.launchService();
            waitMs(20);
        }
    }

    /**
     * Launches a new service
     */
    private void launchService()
    {
        Intent service = new Intent(this, TestService.class);
        startService(service);
    }

    /**
     * Calculates the latency
     * @param start Start time, when execution has started
     * @return Long: latency time (end - start time)
     */
    private double getLatency(double start)
    {
        /* Get the end time */
        double end = System.nanoTime();

        /* Return the latency value in ms */
        return (end - start) / 1000000;
    }

    /**
     * Waits the specified amount of ms
     * @param ms number of ms
     */
    protected void waitMs(int ms)
    {
        Log.d(TAG, "Waiting " + ms + "ms");
        try {
            Thread.sleep(ms);
        } catch(InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * Writes and stores the test log file in the phone storage in the "/test-logs" directory
     * The file name is "testID-test-log.txt"
     * @param testID String
     * @param data String
     */
    private void writeLog(String testID, String data)
    {
        Log.d(TAG, "Writing log for test: " + testID + "...");
        try {
            File sdCard = Environment.getExternalStorageDirectory();
            File dir = new File (sdCard.getAbsolutePath() + "/test-logs");
            File file = new File(dir, testID + ".csv");

            FileOutputStream fout = new FileOutputStream(file);

            fout.write(data.getBytes());

            fout.close();
        } catch(Exception e) {
            Log.e("Exception", "File write failed: " + e.getMessage());
        }
    }

    /**
     * Writes the test results in the phone
     * @param id String
     * @param results List<Double>
     */
    private void writeResults(String id, List<Double> results)
    {
        Log.d(TAG, "Writing file: " + id + "...");

        /* Store the result if it's not empty */
        if(!results.isEmpty()) {
            /* Calculate relevant values */
            double max = Collections.max(results);
            double min = Collections.min(results);
            double mean = Mathematics.mean(results);
            double stDev = Mathematics.stdDev(results);

            writeLog(id, toCsv(results) + "\nMax: " + max + " ms" + "\nMin: " + min +
                    " ms" + "\nMean: " + mean + " ms" + "\nStandard Dev: " + stDev + " ms");
        } else {
            writeLog(id, "Result was empty");
        }
    }

    /**
     * Dumps the event data buffer
     * @param event EventHandler
     * @return ArrayList<Byte>
     */
    private ArrayList<Byte> dumpBuffer(EventHandler event)
    {
        byte result;
        ArrayList<Byte> data = new ArrayList<>();
        do {
            Log.d(TAG, "Reading byte...");
            result = event.dumpBuffer();
            Log.d(TAG, "Read byte!");
            data.add(result);
            waitMs(500);
        } while(result != '\0');

        return data;
    }

    /**
     * Converts a list to csv format
     * @param list List<Double>
     * @return String
     */
    private String toCsv(List<Double> list)
    {
        String csv = "";
        for(double elem : list) {
            /* Replace . by , for correct interpretation in MS Excel */
            csv += String.valueOf(Mathematics.round(elem, 3)).replace(".", ",") + "\n";
        }

        return csv;
    }

}
