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
        /* Do something */
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        String msg = intent.getStringExtra("key");

        Toast.makeText(this, "GoodService starting with: "+msg,
                Toast.LENGTH_SHORT).show();

        return Service.START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

}
