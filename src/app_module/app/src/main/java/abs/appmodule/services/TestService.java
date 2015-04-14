package abs.appmodule.services;

import android.app.Service;
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
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;

import abs.appmodule.R;

public class TestService extends PayloadAPP
{

    private final IBinder mBinder = new LocalBinder();

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
    }

    @Override
    public void onDestroy()
    {
        Toast.makeText(this, "Test done", Toast.LENGTH_SHORT).show();
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
                Toast.makeText(this, "Starting Arduino test...",
                        Toast.LENGTH_SHORT).show();

                /* Get the layout */
                LayoutInflater inflater = (LayoutInflater)
                        getSystemService(LAYOUT_INFLATER_SERVICE);
                View layout = inflater.inflate(R.layout.activity_android, null);

                /* Get the text view and update value dynamically */
                TextView pin_value = (TextView) layout.findViewById(R.id
                        .pin_value);

                Arduino arduino = this.getArduino();
                pin_value.setText(arduino.digitalRead(5));

                writeLog(id, Integer.toString(arduino.digitalRead(5)));

                /* Refresh */
                layout.invalidate();

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
            File file = new File(Environment.getExternalStorageDirectory()
                    .getPath() + testID +"-test-log.txt");

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

}
