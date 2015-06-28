package abs.com.test.appmodule.activities;

import abs.com.test.appmodule.R;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

/**
 * Activity for the latency test
 */
public class LatencyActivity extends PayloadActivity
{
    private static final String TAG = "LatencyActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_latency);
    }

    /**
     * Called when the check button is clicked
     * @param v View
     */
    @Override
    public void onButtonClick(View v)
    {
        /* If the service is bound start the test */
        if(mBound) {
            /* Run the latency test */
            mService.runTest(this.testID);

            TextView valueTv = (TextView) findViewById(R.id.latency_value_label);

            /* Update the text view */
            valueTv.setText("The test has finished");
        }
    }

}