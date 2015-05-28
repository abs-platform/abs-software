package abs.com.test.appmodule.activities;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import abs.com.test.appmodule.R;
import abs.com.test.appmodule.services.TestService;

/**
 * Base class from which all test activities extend
 */
public class PayloadActivity  extends Activity
{
    public TestService mService;
    public boolean mBound = false;
    public String testID;

    /**
     * Variable to manage the service binding
     */
    protected ServiceConnection mConnection;


    public PayloadActivity()
    {
        /* Initialize the binding manager variable overriding some of the ServiceConnection class
         methods */
        mConnection = new ServiceConnection()
        {
            /**
             * Called when the connection with the service is established
             * @param className ComponentName
             * @param service IBinder
             */
            @Override
            public void onServiceConnected(ComponentName className, IBinder service)
            {
                /* Because we have bound to an explicit service that is running in our own process,
                we can cast its IBinder to a concrete class and directly access it */
                mService = ((TestService.LocalBinder) service).getService();
                mBound = true;
            }

            /**
             * Called when the connection with the service disconnects unexpectedly
             * @param className ComponentName
             */
            @Override
            public void onServiceDisconnected(ComponentName className) {
                mBound = false;
            }
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        /* Get the test ID from the extras */
        if(savedInstanceState == null) {
            Bundle extras = getIntent().getExtras();
            if(extras == null)
                this.testID = null;
            else
                this.testID = extras.getString("id");
        } else {
            this.testID = (String) savedInstanceState.getSerializable("id");
        }

        /* Start the service on creating the activity */
        Intent service = new Intent(this, TestService.class);
        startService(service);

        /* Bind to the service so that we can interact with it */
        bindService(service, mConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onStart()
    {
        super.onStart();
    }

    @Override
    protected void onStop()
    {
        super.onStop();
        if(mBound)
        {
            unbindService(mConnection);
            mBound = false;
        }
    }

    /**
     * Called when the check button is clicked
     * @param v View
     */
    public void onButtonClick(View v)
    {
        /* If the service is bound start the test */
        if(mBound) {
            /* Run the test */
            mService.runTest(this.testID);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        int id = item.getItemId();

        return (id == R.id.action_settings) || super.onOptionsItemSelected(item);
    }

}