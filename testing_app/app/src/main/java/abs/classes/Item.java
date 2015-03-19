package abs.classes;

public class Item{

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

}