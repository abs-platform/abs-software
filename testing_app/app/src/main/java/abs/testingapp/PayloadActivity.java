package abs.testingapp;


import abs.services.GoodService;
import abs.services.GoodService.LocalBinder;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.support.v7.app.ActionBarActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;


public class PayloadActivity extends ActionBarActivity {

    public GoodService mService;
    public boolean mBound = false;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_payload);

        /* Get the intent and the passed parameters */
        Intent intent = getIntent();
        String id = intent.getStringExtra("id");
        String title = intent.getStringExtra("title");

        setTitle(title+" test");

        /* Start the service on creating the activity */
        Intent service = new Intent(PayloadActivity.this, GoodService.class);
        /* Pass the item id to the service to know which test to start */
        service.putExtra("id", id);

        startService(service);
        /* Bind to the service so that we can interact with it */
        bindService(service, mConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onStart()
    {
        super.onStart();
    }

    /**
     * Called when the check button is clicked
     * @param v
     */
    public void onButtonClick(View v)
    {
        if (mBound) {
            TextView tv = (TextView) findViewById(R.id.payload_text);
            tv.append("\nService bound");
            /* Call a method from the LocalService. However, if this call were
            something that might hang, then this request should occur in a
            separate thread to avoid slowing down the activity performance. */
            mService.runTest("bat");
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

        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /**
     * Variable to manage the service binding
     */
    private ServiceConnection mConnection = new ServiceConnection() {
        /**
         * Called when the connection with the service is established
         * @param className
         * @param service
         */
        @Override
        public void onServiceConnected(ComponentName className, IBinder service)
        {
            /* Because we have bound to an explicit service that is running
            in our own process, we can cast its IBinder to a concrete class
            and directly access it */
            LocalBinder binder = (LocalBinder) service;
            mService = binder.getService();
            mBound = true;
        }

        /**
         * Called when the connection with the service disconnects unexpectedly
         * @param className
         */
        @Override
        public void onServiceDisconnected(ComponentName className) {
            mBound = false;
        }
    };

}
