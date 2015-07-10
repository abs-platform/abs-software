package abs.com.test.appmodule.services;

import android.app.Service;
import android.content.Intent;
import android.util.Log;

import com.abs.payloadsdk.arduino.Arduino;

/**
 * Class for the Consumer Service on the multiple services test. This Service toggles the value
 * of a pin.
 */
public class ConsumerService extends TestService
{
    private Arduino arduino;
    private final String TAG = "ConsumerService";
    private Thread thread = new Thread() {
        @Override
        public void run()
        {
            /* Perform 1000 reads of a digital pin */
            for(int count=0; count<100000; count++) {
                arduino.digitalToogle(5);
                waitMs(10);
            }
        }
    };

    @Override
    public void onCreate()
    {
        this.arduino = this.getArduino();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        Log.d(TAG, "Reading from digital pin...");

        thread.start();

        stopSelf();

        return Service.START_NOT_STICKY;
    }

}
