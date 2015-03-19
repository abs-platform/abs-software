package abs.classes;

public class Item{

    public String title;

    /**
     * Constructor
     * @param title
     */
    public Item(String title)
    {
        this.title = title;
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