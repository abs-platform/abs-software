package abs.services;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.widget.Toast;

import com.abs.PayloadApp;

public class GoodService extends Service{

    @Override
    public void onCreate()
    {
        /* Do something... or not */
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        String key = intent.getStringExtra("key");

        selectTest(key);

        return Service.START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    /**
     * Does the test selection switching from a given key parameter
     * @param key
     */
    private void selectTest(String key)
    {
        switch(key){
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
