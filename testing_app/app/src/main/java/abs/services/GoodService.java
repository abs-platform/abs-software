package abs.services;

import android.app.Service;
import android.content.Intent;
import android.os.*;
import android.widget.Toast;

import com.abs.PayloadApp;

public class GoodService extends Service{ // Has to extend PayloadApp

    private final IBinder mBinder = new LocalBinder();

    /**
     * Class used for the client Binder. Because we know this service always
     * runs in the same process as its clients, we don't need to deal with IPC.
     */
    public class LocalBinder extends Binder {
        public GoodService getService()
        {
            /* Return this instance of LocalService so clients can call public
            methods */
            return GoodService.this;
        }
    }

    @Override
    public void onCreate()
    {
        /* Start up the thread running the service. We create a separate
        thread because the service normally runs in the process's main
        thread, which we don't want to block. Also background priority so
        CPU-intensive work will not disrupt our UI */
        /*
        HandlerThread thread = new HandlerThread("ServiceStartArguments",
                Process.THREAD_PRIORITY_BACKGROUND);
        thread.start();
        */
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        String id = intent.getStringExtra("id");

        runTest(id);

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
    public boolean onUnbind(Intent intent) {
        /* Return true to allow onRebind */
        return false;
    }
    @Override
    public void onRebind(Intent intent) {
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
        switch(id){
            case "sdb_conn":
                Toast.makeText(this, "Starting SDB test...",
                        Toast.LENGTH_SHORT).show();
                break;
            case "arduino":
                Toast.makeText(this, "Starting Arduino test...",
                        Toast.LENGTH_SHORT).show();
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
                throw new RuntimeException("Incorrect item selection");
        }
    }

}
