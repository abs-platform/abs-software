package abs.com.test.appmodule.services;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.*;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.google.abs.payloadsdk.*;
import com.google.abs.payloadsdk.Arduino.Arduino;

import java.io.File;
import java.io.FileFilter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.RandomAccessFile;

import java.util.regex.Pattern;

import abs.com.test.appmodule.R;

public class TestService extends PayloadApp
{

    private final IBinder mBinder = new LocalBinder();

    public Arduino arduino;

    public Thread thread = new Thread() {
        @Override
        public void run()
        {
            /* Get the layout */
            LayoutInflater inflater = (LayoutInflater)
                    getSystemService(LAYOUT_INFLATER_SERVICE);
            View layout = inflater.inflate(R.layout.activity_arduino, null);

                /* Get the text view and update value dynamically */
            TextView value_tv = (TextView) layout.findViewById(R.id
                    .pin_value);

            try {
                int count = 0;
                while (count < 5) {
                    /* Light the led */
                    arduino.digitalWrite(2, 1);
                    sleep(2000);
                    /* Close the led */
                    arduino.digitalWrite(2, 0);
                    /* Write analog pin value on activity */
                    value_tv.setText(String.valueOf(arduino.analogRead(3)));
                    sleep(2000);

                     /* Refresh */
                    layout.invalidate();

                    count++;
                }
            } catch (InterruptedException ie) {
                ie.printStackTrace();
            }
        }

    };

    /**
     * Class used for the client Binder. Because we know this service always
     * runs in the same process as its clients, we don't need to deal with IPC.
     * -- That's maybe not true and we may want IPC --
     */
    public class LocalBinder extends Binder
    {
        public TestService getService()
        {
            /* Return this instance of LocalService so clients can call public
            methods */
            return TestService.this;
        }
    }

    public TestService()
    {
        super("android-service");
    }

    @Override
    public void onCreate()
    {
        /* If the service performs CPU-intensive work we may want to start
        a new thread with background priority so that we don't block the
        process's main one */
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        Toast.makeText(this, "Starting service...",
                Toast.LENGTH_SHORT).show();

        return Service.START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        Toast.makeText(getApplicationContext(), "binding",
                Toast.LENGTH_SHORT).show();
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
    public void runTest(String id)
    {
        switch(id) {
            case "sdb_conn":
                Toast.makeText(this, "Starting SDB test...",
                        Toast.LENGTH_SHORT).show();
                break;

            case "arduino":
                long start = System.nanoTime();

                this.arduino = this.getArduino();
                this.thread.start();

                long end = System.nanoTime();

                /* Calculate and write on a log file the execution time */
                writeLog(id, "I took: " + ((end - start) / 1000000) + "ms");

                break;

            case "bat":
                Toast.makeText(this, "Starting battery state test...",
                        Toast.LENGTH_SHORT).show();
                break;

            case "mem":
                Toast.makeText(this, "Starting memory usage test...",
                        Toast.LENGTH_SHORT).show();
                break;

            case "cpu":
                Toast.makeText(this, "Starting CPU test...",
                        Toast.LENGTH_SHORT).show();
                break;

            default:
                throw new RuntimeException("Incorrect test ID");
        }

        stopSelf();
    }

    /**
     * Writes and stores the test log file on the external storage device (i.e.
     * SD card)
     * @param testID
     * @param data
     */
    private void writeLog(String testID, String data)
    {
        try {
            File file = new File(this.getFilesDir() + "/test-logs/" + testID
                    +"-test-log.txt");

            if (file.createNewFile()) {
                FileOutputStream fout = new FileOutputStream(file);
                OutputStreamWriter outWriter = new OutputStreamWriter(fout);

                outWriter.write(data);

                outWriter.close();
                fout.close();
            }
        } catch (Exception e) {
            Log.e("Exception", "File write failed: " + e.getMessage());
        }
    }

    /**
     * Gets the number of CPU's of the processor
     * @return
     */
    private int getNumOfCpus()
    {
        class CPUFilter implements FileFilter
        {
            @Override
            public boolean accept(File pathname)
            {
                // Check if filename is "cpu0", "cpu1,...
                return Pattern.matches("cpu[0-9]", pathname.getName());
            }
        }

        try {
            // Get directory containing CPU info
            File dir = new File("/sys/devices/system/cpu/");
            File[] files = dir.listFiles(new CPUFilter());
            // Return the number of cores
            return files.length;
        } catch (Exception e) {
            e.printStackTrace();
            return 1;
        }
    }

    /**
     * Calculates the cpu usage. It is interesting to call this function in
     * some intervals (like 5 seconds) and write the results on the log file.
     * We will end up having a list of cpu usage throughout the whole app
     * usage time
     */
    /**
     * Implement this for as many cpu's as we have with the above function
     * @return
     */
    private float cpuUsage()
    {
        try {
            RandomAccessFile reader = new RandomAccessFile("/proc/stat", "r");
            String load = reader.readLine();

            String[] toks = load.split(" +");  // Split on one or more spaces

            long idle1 = Long.parseLong(toks[4]);
            long cpu1 = Long.parseLong(toks[2]) + Long.parseLong(toks[3]) +
                    Long.parseLong(toks[5]) + Long.parseLong(toks[6]) +
                    Long.parseLong(toks[7]) + Long.parseLong(toks[8]);

            try {
                Thread.sleep(360);
            } catch (Exception e) {
                e.printStackTrace();
            }

            reader.seek(0);
            load = reader.readLine();
            reader.close();

            toks = load.split(" +");

            long idle2 = Long.parseLong(toks[4]);
            long cpu2 = Long.parseLong(toks[2]) + Long.parseLong(toks[3]) +
                    Long.parseLong(toks[5]) + Long.parseLong(toks[6]) +
                    Long.parseLong(toks[7]) + Long.parseLong(toks[8]);

            return (float)(cpu2 - cpu1) / ((cpu2 + idle2) - (cpu1 + idle1));

        } catch (IOException ex) {
            ex.printStackTrace();
        }

        return 0;
    }

}
