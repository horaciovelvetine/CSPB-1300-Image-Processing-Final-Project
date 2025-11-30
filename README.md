# CSPB-1300 Final: Image Processing Project

<hr/>

Create a command line interface application which can apply 10 image processing functions on the provided `.bmp` images in the [sample_images](./sample_images/) directory. There is existing code inside the [main.cpp](./main.cpp) which provides some functions already implemented which are:

- (struct) `Pixel` {red: int, green: int, blue: int}
- (func) `get_int(fstream& stream, int offset, int bytes)` :: Helper for `read_image()` **(can be safely ignored)**
- (func) `read_image(string filename)` :: Read(s) the specified BMP image and returns a `vector<vector<Pixel>>` representing the image
- (func) `set_bytes(unsigned char arr[], int offset, int bytes, int value)` :: Helper for the `write_image()` **(can be safely ignored)**
- (func) `write_image(string filename, const vector<vector<Pixel>>& image)` :: Write the provided image vectory to a BMP file. Uses the provided filename for the OP file. 
  - **NOTE: Don't forget to include the '.bmp' file-extension, otherwsie this wont function will not work**
  
<hr/>

## Processes (TODO):

Each process should:
- Take in the original image as a parameter
- Create and Initialize a new 2D `<vector<vector<Pixel>>` to store modified image data
- Iterate through the original image
- Perform the request image processing on each pixel
- Save the new pixel values to the modified image data vector
- Return the new image data vector

The following is a summary of each of the required processes...

- [ ] #1 Vignette Effect (darkens corners)
  ```c++ 
    vector<vector<Pixel>> process_1(const vector<vector<Pixel>>& image)
	``` 

- [ ] #2 Clarendon Effect (darks darker, lights lighter by a scale factor)
  ```c++ 
    vector<vector<Pixel>> process_2(const vector<vector<Pixel>>& image, double scaling_factor)
	``` 

- [ ] #3 Grayscale Image 
  ```c++ 
    vector<vector<Pixel>> process_3(const vector<vector<Pixel>>& image)
	``` 

- [ ] #4 Rotate (clockwise) by 90 degrees
  ```c++ 
    vector<vector<Pixel>> process_4(const vector<vector<Pixel>>& image)
	``` 

- [ ] #5 Rotate (clockwise) by specified multiple (of 90 degrees)
  ```c++ 
    vector<vector<Pixel>> process_5(const vector<vector<Pixel>>& image, int number)
	``` 

- [ ] #6 Enlarge image in the x and y direction
  ```c++ 
    vector<vector<Pixel>> process_6(const vector<vector<Pixel>>& image, int x_scale, int y_scale)
	``` 

- [ ] #7 Convert image to high contrast (black and white only)
  ```c++ 
    vector<vector<Pixel>> process_7(const vector<vector<Pixel>>& image) 
	``` 

- [ ] #8 Lighten by a scaling factor
  ```c++ 
    vector<vector<Pixel>> process_8(const vector<vector<Pixel>>& image, double scaling_factor) 
	``` 

- [ ] #9 Darken by a scaling factor
  ```c++ 
    vector<vector<Pixel>> process_9(const vector<vector<Pixel>>& image, double scaling_factor)  
	``` 

- [ ] #10 Filter to limited color channels: Red, Blue, Green, White, Black
  ```c++ 
    vector<vector<Pixel>> process_10(const vector<vector<Pixel>>& image)
	``` 

## User Interface (TODO):

Requirements for the application UI, should use the CLI for operation

- [ ] User can enter input BMP filename (the image that operations will be performed on)
- [ ] Displays a menu of available selections to the user
  - [ ] Should include option to change the BMP file being modified
  - [ ] Include options for each of 10 processes
  - [ ] Include an exit option
- [ ] User can enter desired process / menu selection (???)
- [ ] User can enter the desired output BMP filename
- [ ] User can enter the (needed) parameters for the selected operation
  - **NOTE: Applies to: 2, 5, 6, 8, 9**
- [ ] Displays confirmation of successful operation, or an Error for a failed operation
- [ ] User is brought back to main menu after each operation so they can perform additional operations (new image, apply additional processing, exit...)
- [ ] User can (gracefully) exit the program

<hr/****>

## Process Reference Code:

### Process 1: Vignette

```python
def process1(img):
    width=img.getWidth()
    height=img.getHeight()
    newimg = image.EmptyImage(width,height)
    for row in range(height):
        for col in range(width):
            p = img.getPixel(col, row)
            # find the distance to the center
            distance = math.sqrt((col - width/2)**2+(row - height/2)**2)
            scaling_factor = (height - distance)/height
            newred = p.getRed()*scaling_factor
            newgreen = p.getGreen()*scaling_factor
            newblue = p.getBlue()*scaling_factor
            #set new pixel to new color values.
            newpixel = image.Pixel(newred, newgreen, newblue)
            newimg.setPixel(col, row, newpixel)

    newwin=image.ImageWin(width,height)
    newimg.draw(newwin)

    return newimg
```

### Process 2: Claredon

```python
def process2(img):
    width=img.getWidth()
    height=img.getHeight()
    scaling_factor = 0.3
    newimg = image.EmptyImage(width,height)
    for row in range(height):
        for col in range(width):
            p = img.getPixel(col, row)
            #What are the red, blue and green values for p?
            red_value = p.getRed()
            green_value = p.getGreen()
            blue_value = p.getBlue()

            #average those values
            average_value = (red_value + green_value + blue_value)/3

            # if the cell is light, make it lighter
            if average_value >= 170:
                newred = int(255 - (255 - red_value)*scaling_factor)
                newgreen = int(255 - (255 - green_value)*scaling_factor)
                newblue =  int(255 - (255 - blue_value)*scaling_factor)
            elif average_value < 90:
                newred = red_value*scaling_factor
                newgreen = green_value*scaling_factor
                newblue =  blue_value*scaling_factor
            else:
                newred = red_value
                newgreen = green_value
                newblue =  blue_value

            #set new pixel to new color values.
            newpixel = image.Pixel(newred, newgreen, newblue)

            newimg.setPixel(col, row, newpixel)

    newwin=image.ImageWin(width,height)
    newimg.draw(newwin)
    return newimg
```

### Process 3: Grayscale

```python
def process3(img):
    #get the height and width of the image
    width=img.getWidth()
    height=img.getHeight()
    newimg = image.EmptyImage(width,height)
    for row in range(height):
        for col in range(width):
            p = img.getPixel(col, row)
            #What are the red, blue and green values for p?
            red_value = p.getRed()
            green_value = p.getGreen()
            blue_value = p.getBlue()

            # average those values to get the grey value
            gray_value = (red_value + green_value + blue_value)/3

            # set new color values to all be our grey value
            newred = gray_value
            newgreen = gray_value
            newblue = gray_value

            #set new pixel to new color values.
            newpixel = image.Pixel(newred, newgreen, newblue)

            newimg.setPixel(col, row, newpixel)
    # then draw the new images
    newwin=image.ImageWin(width,height)
    newimg.draw(newwin)
    return newimg
```

### Process 4: Rotate 90 (clockwise)

```python
def process4(img):
    width=img.getWidth()
    height=img.getHeight()
    newimg = image.EmptyImage(height,width) #fresh canvas for rotated image
    for row in range(height):
        for col in range(width):
            p = img.getPixel(col, row)
            newimg.setPixel( (height-1) - row,col,p)
    newwin=image.ImageWin(width,height)
    newimg.draw(newwin)
    return newimg
```
### Process 5: Rotate multiples of 90 (clockwise)

```python
def rotate_by_90(img):
    width=img.getWidth()
    height=img.getHeight()
    newimg = image.EmptyImage(height,width) #fresh canvas for rotated image

    for row in range(height):
        for col in range(width):
            p = img.getPixel(col, row)
            newimg.setPixel( (height-1) - row,col,p)
    newwin=image.ImageWin(width,height)
    newimg.draw(newwin)
    return newimg

def process5(img, number):
    angle = int(number*90)
    if angle%90 != 0:
        print("angle must be a multiple of 90 degrees.")
    elif angle%360 == 0:
        return img
    elif angle%360 == 90:
        return rotate_by_90(img)
    elif angle%360 == 180:
        return rotate_by_90(rotate_by_90(img))
    else:
    		return rotate_by_90(rotate_by_90(rotate_by_90(img)))
```

### Process 6: Enlarge on X & Y axis (with scale(s))

```python
def process6(img,xscale,yscale):
    width = img.getWidth()
    height = img.getHeight()
    newimg = image.EmptyImage(xscale*width,yscale*height)
    win = image.ImageWin(width,height)
    img.draw(win)
    for row in range(int(yscale*height)):
        for col in range(int(xscale*width)):
            p = img.getPixel(int(col/xscale),int(row/yscale))
            newimg.setPixel(col,row,p)
    return newimg
```
### Process 7: Convert to high contrase (black and white)

```python
def process7(img):
    #get the height and width of the image
    width=img.getWidth()
    height=img.getHeight()
    newimg = image.EmptyImage(width,height)
    for row in range(height):
        for col in range(width):
            p = img.getPixel(col, row)
            #What are the red, blue and green values for p?
            red_value = p.getRed()
            green_value = p.getGreen()
            blue_value = p.getBlue()

            # average those values to get the grey value
            gray_value = (red_value + green_value + blue_value)/3

            if gray_value >= 255/2:
                newred = 255
                newgreen = 255
                newblue = 255
            else:
                newred = 0
                newgreen = 0
                newblue = 0

            #set new pixel to new color values.
            newpixel = image.Pixel(newred, newgreen, newblue)

            newimg.setPixel(col, row, newpixel)
    # then draw the new images
    newwin=image.ImageWin(width,height)
    newimg.draw(newwin)
    return newimg
```

### Process 8: Lighten by scale

```python
def process8(img):
    #get the height and width of the image
    width=img.getWidth()
    height=img.getHeight()
    newimg = image.EmptyImage(width,height)
    for row in range(height):
        for col in range(width):
            p = img.getPixel(col, row)
            #What are the red, blue and green values for p?
            red_value = p.getRed()
            green_value = p.getGreen()
            blue_value = p.getBlue()

            scaling_factor = .8
            newred = int(255 - (255 - red_value)*scaling_factor)
            newgreen = int(255 - (255 - green_value)*scaling_factor)
            newblue = int(255 - (255 - blue_value)*scaling_factor)

            #set new pixel to new color values.
            newpixel = image.Pixel(newred, newgreen, newblue)

            newimg.setPixel(col, row, newpixel)
    # then draw the new images
    newwin=image.ImageWin(width,height)
    newimg.draw(newwin)
    return newimg
```

### Process 9: Darken by scale

```python
def process9(img):
    #get the height and width of the image
    width=img.getWidth()
    height=img.getHeight()
    newimg = image.EmptyImage(width,height)
    for row in range(height):
        for col in range(width):
            p = img.getPixel(col, row)
            #What are the red, blue and green values for p?
            red_value = p.getRed()
            green_value = p.getGreen()
            blue_value = p.getBlue()

            scaling_factor = .8
            newred = red_value*scaling_factor
            newgreen = green_value*scaling_factor
            newblue = blue_value*scaling_factor

            #set new pixel to new color values.
            newpixel = image.Pixel(newred, newgreen, newblue)

            newimg.setPixel(col, row, newpixel)
    # then draw the new images
    newwin=image.ImageWin(width,height)
    newimg.draw(newwin)
    return newimg
```

### Process 10: Filter to RGB+BLK & WHITE color channels

```python
def process10(img):
    #get the height and width of the image
    width=img.getWidth()
    height=img.getHeight()
    newimg = image.EmptyImage(width,height)
    for row in range(height):
        for col in range(width):
            p = img.getPixel(col, row)
            #What are the red, blue and green values for p?
            red_value = p.getRed()
            green_value = p.getGreen()
            blue_value = p.getBlue()
            max_color = max(red_value, blue_value, green_value)
            if red_value + green_value + blue_value >= 550:
                newred = 255
                newgreen = 255
                newblue = 255
            elif red_value + green_value + blue_value <= 150:
                newred = 0
                newgreen = 0
                newblue = 0
            elif max_color == red_value:
                newred = 255
                newgreen = 0
                newblue = 0
            elif max_color == green_value:
                newred = 0
                newgreen = 255
                newblue = 0
            else:
                newred = 0
                newgreen = 0
                newblue = 255

            #set new pixel to new color values.
            newpixel = image.Pixel(newred, newgreen, newblue)

            newimg.setPixel(col, row, newpixel)
    # then draw the new images
    newwin=image.ImageWin(width,height)
    newimg.draw(newwin)
    return newimg
```

## Refernces: 

- [Rotating a Bitmap Image](https://www.leunen.com/cbuilder/rotbmp.html)
- [How can I rotate a bitmap?](https://gamedev.stackexchange.com/questions/67613/how-can-i-rotate-a-bitmap-without-d3d-or-opengl)
- [Rotate a bitmap image](https://www.codeguru.com/multimedia/rotate-a-bitmap-image/)
- [x86-bmp-image-rotation](https://github.com/cywinski/x86-bmp-image-rotation)
- [How do I rotate a bitmap image?](https://math.stackexchange.com/questions/3207791/how-do-i-rotate-a-bitmap-image)
- [Rotating Images](http://datagenetics.com/blog/august32013/index.html)