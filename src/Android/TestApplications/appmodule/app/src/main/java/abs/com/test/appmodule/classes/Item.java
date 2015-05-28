package abs.com.test.appmodule.classes;

/**
 * Class for the items on the MainActivity ListView
 */
public class Item
{

    public String id;
    public String title;

    /**
     * Constructor
     * @param title
     */
    public Item(String id, String title)
    {
        this.id = id;
        this.title = title;
    }

    /**
     * Sets the id of the list item
     * @param id
     */
    public void set_id(String id)
    {
        this.id = id;
    }

    /**
     * Gets the id of the list item
     * @return String
     */
    public String get_id()
    {
        return this.id;
    }

    /**
     * Sets the title of the list item
     * @param title
     */
    public void set_title(String title)
    {
        this.title = title;
    }

    /**
     * Gets the title of the list item
     * @return String
     */
    public String get_title()
    {
        return this.title;
    }

    /**
     * Gets the activity package classpath to be run when an item is clicked
     * @return The name of the activity package path
     */
    public String get_activity()
    {
        if(this.get_activity_name() != null)
            return "abs.com.test.appmodule.activities." + this.get_activity_name();

        return null;
    }

    /**
     * Gets the activity name
     * @return The name of the activity
     */
    public String get_activity_name()
    {
        String id = this.get_id();

        if(this.get_id().length() > 1) {
            /* Make the first letter of the name uppercase to match the name
            of the file */
            String capId = id.substring(0, 1).toUpperCase() + id.substring(1);

            return capId + "Activity";
        }

        return null;
    }

}