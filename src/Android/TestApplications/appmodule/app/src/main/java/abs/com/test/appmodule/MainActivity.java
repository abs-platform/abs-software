package abs.com.test.appmodule;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;

import abs.com.test.appmodule.classes.Item;
import abs.com.test.appmodule.classes.ItemAdapter;
import abs.com.test.appmodule.services.TestService;


public class MainActivity extends Activity
{
    public ListView listView;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.listView = (ListView) findViewById(R.id.listView);

        List<Item> items = createList();
        this.listView.setAdapter(new ItemAdapter(this, items));

        /* Set the click listener for listView items and start the service if
         the user clicks one of them */
        listView.setOnItemClickListener(itemClickHandler());
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        getMenuInflater().inflate(R.menu.menu_main, menu);
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

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        stopService(new Intent(this, TestService.class));
    }

    /**
     * Creates and populates the list of items
     * @return List<Item>
     */
    private List<Item> createList()
    {
        List<Item> items = new ArrayList<>();

        items.add(new Item("latency", "Latency"));
        items.add(new Item("latencydata", "Latency with variable data"));
        items.add(new Item("events", "Event collision"));
        items.add(new Item("services", "Multiple services"));
        items.add(new Item("app-msg", "Inter-app messages"));

        return items;
    }

    /**
     * Handler method for the click event on any of the list items
     */
    private AdapterView.OnItemClickListener itemClickHandler() {
        return new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id)
            {
                /* Identify the item that is clicked */
                Item item = (Item) parent.getItemAtPosition(position);

                /* Start the corresponding activity */
                try {
                    Class c = Class.forName(item.get_activity());
                    Intent intent = new Intent(MainActivity.this, c);
                    /* Get the item id and pass it to the activity to know which test to start */
                    intent.putExtra("id", item.get_id());
                    intent.putExtra("activity", item.get_activity());

                    startActivity(intent);
                } catch (ClassNotFoundException e) {
                    System.out.println("Class not found: " + e.getMessage());
                }
            }
        };
    }
}