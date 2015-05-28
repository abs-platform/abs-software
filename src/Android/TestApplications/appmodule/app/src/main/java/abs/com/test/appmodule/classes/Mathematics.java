package abs.com.test.appmodule.classes;

import java.util.*;

public class Mathematics
{
    /**
     * Rounds the number to the number of decimals specified
     * @param num double
     * @param decimals int
     * @return double
     */
    public static double round(double num, int decimals)
    {
        return Math.round(num*Math.pow(10, decimals))/Math.pow(10, decimals);
    }

    /**
     * Sums all the elements of a list
     * @param a List<Double>
     * @return double
     */
    public static double sum(List<Double> a)
    {
        if(a.size() > 0) {
            double sum = 0;

            for(Double i : a) {
                sum += i;
            }
            return sum;
        }

        return 0;
    }

    /**
     * Calculates the mean of a list
     * @param a List<Double>
     * @return double
     */
    public static double mean(List<Double> a)
    {
        return sum(a) / (a.size() * 1.0);
    }

    /**
     * Calculates the median of a list
     * @param a List<Double>
     * @return double
     */
    public static double median(List<Double> a)
    {
        int middle = a.size()/2;

        if(a.size() % 2 == 1)
            return a.get(middle);
        else
            return (a.get(middle-1) + a.get(middle)) / 2.0;
    }

    /**
     * Calculates the standard deviation of a list
     * @param a List<Double>
     * @return double
     */
    public static double stdDev(List<Double> a)
    {
        int sum = 0;
        double mean = mean(a);

        for (Double i : a)
            sum += Math.pow((i - mean), 2);

        return Math.sqrt( sum / ( a.size() - 1 ) );
    }
}