package abs.services;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Process;
import android.widget.Toast;

import com.abs.PayloadApp;

public class DemoService extends Service { // has to extend from PayloadApp

    public int mStartMode;  // Indicates how to behave if the service is killed
    public IBinder mBinder; // Interface for clients that bind
    public boolean mAllowRebind;

    private Looper _mServiceLooper;
    private ServiceHandler _mServiceHandler;

    /**
     * Handler class that receives messages from the thread
     */
    private final class ServiceHandler extends Handler {
        /**
         * Constructor
         * @param looper
         */
        public ServiceHandler(Looper looper)
        {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg)
        {
            /* Normally we would do some work here, like download a file */
            /* For our sample, we just sleep for 5 seconds */
            long endTime = System.currentTimeMillis() + 5*1000;
            while (System.currentTimeMillis() < endTime) {
                synchronized (this) {
                    try {
                        wait(endTime - System.currentTimeMillis());
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
            // Stop the service using the startId, so that we don't stop
            // the service in the middle of handling another job
            stopSelf(msg.arg1);
        }
    }

    /**
     * First function called on a start of a service, before any other
     */
    public void onCreate()
    {
        /* If we get killed, after returning from onStartCommand, restart */
        mStartMode = START_STICKY;

        /* Start up the thread running the service.  Note that we create a
        separate thread because the service normally runs in the process's
        main thread, which we don't want to block.  We also make it
        background priority so CPU-intensive work will not disrupt our UI */
        HandlerThread thread = new HandlerThread("ServiceStartArguments",
                Process.THREAD_PRIORITY_BACKGROUND);
        thread.start();

        /* Get the HandlerThread's Looper and use it for our Handler */
        _mServiceLooper = thread.getLooper();
        _mServiceHandler = new ServiceHandler(_mServiceLooper);
    }

    /**
     * The system calls this method when startService() is called and after
     * calling onCreate()
     * @param intent
     * @param flags
     * @param startId
     */
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // Show a message that service has started
        Toast.makeText(this, "Demo service starting", Toast.LENGTH_SHORT).show();

        // For each start request, send a message to start a job and deliver the
        // start ID so we know which request we're stopping when we finish the job
        Message msg = _mServiceHandler.obtainMessage();
        msg.arg1 = startId;
        _mServiceHandler.sendMessage(msg);

        return mStartMode;
    }

    /**
     * The system calls this method when another component wants to bind with the
     * service by calling bindService()
     * @param intent
     * @return IBinder
     */
    public IBinder onBind(Intent intent)
    {
        return mBinder;
    }

    /**
     * All clients have unbound with unbindService()
     * @param intent
     * @return boolean
     */
    public boolean onUnbind(Intent intent)
    {
        return mAllowRebind;
    }

    /**
     * A client is binding to the service with bindService(), after onUnbind() has
     * already been called
     * @param intent
     */
    public void onRebind(Intent intent)
    {

    }

    /**
     * The service is no longer being used and is being destroyed
     */
    public void onDestroy()
    {

    }

}