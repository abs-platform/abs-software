Test 1 - Draw a circle in an image an save the results

	Mat imageData = imread("background.png", CV_LOAD_IMAGE_COLOR);
	Rect rectangle1 = Rect(MAX_ROWS/4, MAX_COLS/4, 30, 30);
  	CircleData c1;
	rect2circle(rectangle1, c1);
	drawCircle(imageData, c1); 
	waitKey(0);
	imwrite("result.png",imageData);


Test 2 - Compute the velocity of a satellite (using circles)

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
