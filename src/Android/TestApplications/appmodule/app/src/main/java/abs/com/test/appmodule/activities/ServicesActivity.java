package abs.com.test.appmodule.activities;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import abs.com.test.appmodule.R;
import abs.com.test.appmodule.services.ConsumerService;

/**
 * Activity for the latency with multiple services test
 */
public class ServicesActivity extends PayloadActivity
{
    private final String TAG = "ServicesActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_services);
    }

    /**
     * Called when the check button is clicked
     * @param v View
     */
    public void onMeasButtonClick(View v)
    {
        /* If the service is bound start the test */
        if(mBound) {
            Log.d(TAG, "Starting latency test...");
            /* Run the latency test */
            mService.runTest("services");

            TextView valueTv = (TextView) findViewById(R.id.test_end_label);

            /* Update the text view */
            valueTv.setText("The test has finished!");
        }
    }

    /**
     * Called when the check button is clicked
     * @param v View
     */
    public void onConsumerButtonClick(View v)
    {
        /* Start the service */
        Log.d(TAG, "Starting consumer service...");
        Intent service = new Intent(ServicesActivity.this, ConsumerService.class);
        startService(service);
        Log.d(TAG, "Consumer service started!");

        TextView valueTv = (TextView) findViewById(R.id.started_services);

        /* Update the text view */
        valueTv.append("\nConsumer Service started!");
    }

}
