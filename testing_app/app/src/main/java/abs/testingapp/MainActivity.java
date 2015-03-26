package abs.testingapp;

import android.content.Intent;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;

import abs.classes.Item;
import abs.classes.ItemAdapter;

public class MainActivity extends ActionBarActivity {

    public ListView listView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.listView = (ListView) findViewById(R.id.listView);

        List<Item> items = createList();
        this.listView.setAdapter(new ItemAdapter(this, items));

        /* Set the click listener for listView items and start the
        service if the user clicks one of them */
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                /* Identify the item that is clicked */
                Item item = (Item) parent.getItemAtPosition(position);

                /* Start the corresponding activity */
                try {
                    Class c = Class.forName(item.get_activity());
                    Intent intent = new Intent(MainActivity.this, c);
                    /* Get the item id and pass it to the activity to know which
                    test to start */
                    intent.putExtra("id", item.get_id());

                    startActivity(intent);
                } catch(ClassNotFoundException e){
                    System.out.println("Class not found: " + e.getMessage());
                }
            }
        });
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /**
     * Creates and populates the list of items
     * @return List<Item>
     */
    private List<Item> createList()
    {
        List<Item> items = new ArrayList<>();

        items.add(new Item("sdb", "SDB connection"));
        items.add(new Item("arduino", "Arduino"));
        items.add(new Item("battery", "Battery State"));
        items.add(new Item("memory", "Memory Usage"));
        items.add(new Item("cpu", "CPU Usage"));

        return items;
    }
}