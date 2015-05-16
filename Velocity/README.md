# Velocity estimation
This program is the first step of the global velocity estimation algorithm.
This code has been crafted to test three things:

0. Draw a circle in an image and save the results in a file system.
0. Compute the velocity of a satellite (using circles) in each of the three axes.
0. Compute the velocity of a satellite (using rectangles) in each of the three axes and compute how many time is needed by the algorithm to estimate the velocity.

> To do any test shown below, you only need to copy the code and paste it in the main of the main.cpp file. This program has been tested in Ubuntu 64bit 14.04 with openCV 2.3.1 and images of size 120x120px. Make sure you have already installed OpenCV 2.3.1 on your computer, specifically in your personal folder. Otherwise, you can download it [here](http://opencv.org/downloads.html).

<br>

### Test 1 - Draw a circle in an image an save the results
This algorithm receives the parameters of the square in which the detection algorithm finds the satellite. Since the velocity estimation works with circles, a conversion from rectangle to circle is made. After that, it draws a circle in an image and save the results. (To do the test, the corresponding parameters will be defined manually)

```
Mat imageData = imread("background.png", CV_LOAD_IMAGE_COLOR);
Rect rectangle1 = Rect(MAX_ROWS/4, MAX_COLS/4, 30, 30);
CircleData c1;
rect2circle(rectangle1, c1);
drawCircle(imageData, c1); 
waitKey(0);
imwrite("result.png",imageData);
```
<br>

### Test 2 - Compute the velocity of a satellite (using circles)
This algorithm receives the parameters of the circles in which the detection algorithm finds the satellite of the first and the second photograph taken by the mobile phone camera. Considering both circles, it computes the velocity in each axis and shows the results.

```
float sample_time = 2.0;		//Sample time in miliseconds

CircleData c1;
c1.center = Point(60,60);
c1.radius = 20;

CircleData c2;
c2.center = Point(70,40);
c2.radius = 120;
	
Velocity velocity;
velocityXYZ(velocity, c1, c2, sample_time);
 		
printf("Velocity in X: %.2f m/s\n", velocity.velX);
printf("Velocity in Y: %.2f m/s\n", velocity.velY);
printf("Velocity in Z: %.2f m/s\n", velocity.velZ);
```

<br>

### Test 3 - Compute velocity (using rectangles) and time elapsed
This algorithm receives the parameters of the rectangles in which the detection algorithm finds the satellite of the first and the second photograph taken by the mobile phone camera. Then, a conversion from rectangle to circle is made. Considering both circles, it computes the velocity in each axis. Moreover, it computes the time elapsed to estimate the velocity. Finally, it shows the results regarding both the velocity and the time elapsed.

```
double time_elapsed;
float sample_time = 2.0;	//Sample time in miliseconds

Rect rectangle1 = Rect(MAX_ROWS/4, MAX_COLS/4, 30, 30);
Rect rectangle2 = Rect(MAX_ROWS/2, MAX_COLS/4, 40, 40);
	
CircleData c1;
CircleData c2;
rectToCircle(rectangle1, c1);
rectToCircle(rectangle2, c2);

Velocity velocity;
clock_t begin = clock();
velocityXYZ(velocity, c1, c2, sample_time);
clock_t end = clock();
	
time_elapsed = double(end - begin) / CLOCKS_PER_SEC;

printf("Velocity in X: %.2f m/s\n", velocity.velX);
printf("Velocity in Y: %.2f m/s\n", velocity.velY);
printf("Velocity in Z: %.2f m/s\n", velocity.velZ);
printf("Time elapsed to compute the velocity: %.1lf us\n", time_elapsed*1e6);
```
