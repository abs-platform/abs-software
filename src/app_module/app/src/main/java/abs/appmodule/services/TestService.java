package abs.appmodule.services;

import android.app.Service;
import android.content.Intent;
import android.os.*;
import android.widget.Toast;

import com.google.abs.payloadsdk.*;

/* This is incorrect because it has to extend PayloadApp but as there were
some errors with SDK library (which is not complete) I left it like this */
public class TestService extends Service
{

    private final IBinder mBinder = new LocalBinder();

    /**
     * Class used for the client Binder. Because we know this service always
     * runs in the same process as its clients, we don't need to deal with IPC.
     * -- That's maybe not true and we MAY want IPC --
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

}
