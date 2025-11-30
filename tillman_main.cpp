/*
main.cpp
CSPB 1300 Image Processing Application

PLEASE FILL OUT THIS SECTION PRIOR TO SUBMISSION

- Your name:
    James Tillman

- All project requirements fully met? (YES or NO):
    YES

- If no, please explain what you could not get to work:
    N/A

- Did you do any optional enhancements? If so, please explain:
    - Handled the user not including the .bmp file extension for either selecting or outputting a result file
    - added process_11 to rotate the provided picture by an arbitrary amount

*/

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
using namespace std;

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION BELOW                                    //
//***************************************************************************************************//

// Pixel structure
struct Pixel
{
    // Red, green, blue color values
    int red;
    int green;
    int blue;
};

/**
 * Gets an integer from a binary stream.
 * Helper function for read_image()
 * @param stream the stream
 * @param offset the offset at which to read the integer
 * @param bytes  the number of bytes to read
 * @return the integer starting at the given offset
 */
int get_int(fstream &stream, int offset, int bytes)
{
    stream.seekg(offset);
    int result = 0;
    int base = 1;
    for (int i = 0; i < bytes; i++)
    {
        result = result + stream.get() * base;
        base = base * 256;
    }
    return result;
}

/**
 * Reads the BMP image specified and returns the resulting image as a vector
 * @param filename BMP image filename
 * @return the image as a vector of vector of Pixels
 */
vector<vector<Pixel>> read_image(string filename)
{
    // Open the binary file
    fstream stream;
    stream.open(filename, ios::in | ios::binary);

    // Get the image properties
    int file_size = get_int(stream, 2, 4);
    int start = get_int(stream, 10, 4);
    int width = get_int(stream, 18, 4);
    int height = get_int(stream, 22, 4);
    int bits_per_pixel = get_int(stream, 28, 2);

    // Scan lines must occupy multiples of four bytes
    int scanline_size = width * (bits_per_pixel / 8);
    int padding = 0;
    if (scanline_size % 4 != 0)
    {
        padding = 4 - scanline_size % 4;
    }

    // Return empty vector if this is not a valid image
    if (file_size != start + (scanline_size + padding) * height)
    {
        return {};
    }

    // Create a vector the size of the input image
    vector<vector<Pixel>> image(height, vector<Pixel>(width));

    int pos = start;
    // For each row, starting from the last row to the first
    // Note: BMP files store pixels from bottom to top
    for (int i = height - 1; i >= 0; i--)
    {
        // For each column
        for (int j = 0; j < width; j++)
        {
            // Go to the pixel position
            stream.seekg(pos);

            // Save the pixel values to the image vector
            // Note: BMP files store pixels in blue, green, red order
            image[i][j].blue = stream.get();
            image[i][j].green = stream.get();
            image[i][j].red = stream.get();

            // We are ignoring the alpha channel if there is one

            // Advance the position to the next pixel
            pos = pos + (bits_per_pixel / 8);
        }

        // Skip the padding at the end of each row
        stream.seekg(padding, ios::cur);
        pos = pos + padding;
    }

    // Close the stream and return the image vector
    stream.close();
    return image;
}

/**
 * Sets a value to the char array starting at the offset using the size
 * specified by the bytes.
 * This is a helper function for write_image()
 * @param arr    Array to set values for
 * @param offset Starting index offset
 * @param bytes  Number of bytes to set
 * @param value  Value to set
 * @return nothing
 */
void set_bytes(unsigned char arr[], int offset, int bytes, int value)
{
    for (int i = 0; i < bytes; i++)
    {
        arr[offset + i] = (unsigned char)(value >> (i * 8));
    }
}

/**
 * Write the input image to a BMP file name specified
 * @param filename The BMP file name to save the image to
 * @param image    The input image to save
 * @return True if successful and false otherwise
 */
bool write_image(string filename, const vector<vector<Pixel>> &image)
{
    // Get the image width and height in pixels
    int width_pixels = image[0].size();
    int height_pixels = image.size();

    // Calculate the width in bytes incorporating padding (4 byte alignment)
    int width_bytes = width_pixels * 3;
    int padding_bytes = 0;
    padding_bytes = (4 - width_bytes % 4) % 4;
    width_bytes = width_bytes + padding_bytes;

    // Pixel array size in bytes, including padding
    int array_bytes = width_bytes * height_pixels;

    // Open a file stream for writing to a binary file
    fstream stream;
    stream.open(filename, ios::out | ios::binary);

    // If there was a problem opening the file, return false
    if (!stream.is_open())
    {
        return false;
    }

    // Create the BMP and DIB Headers
    const int BMP_HEADER_SIZE = 14;
    const int DIB_HEADER_SIZE = 40;
    unsigned char bmp_header[BMP_HEADER_SIZE] = {0};
    unsigned char dib_header[DIB_HEADER_SIZE] = {0};

    // BMP Header
    set_bytes(bmp_header, 0, 1, 'B');                                             // ID field
    set_bytes(bmp_header, 1, 1, 'M');                                             // ID field
    set_bytes(bmp_header, 2, 4, BMP_HEADER_SIZE + DIB_HEADER_SIZE + array_bytes); // Size of BMP file
    set_bytes(bmp_header, 6, 2, 0);                                               // Reserved
    set_bytes(bmp_header, 8, 2, 0);                                               // Reserved
    set_bytes(bmp_header, 10, 4, BMP_HEADER_SIZE + DIB_HEADER_SIZE);              // Pixel array offset

    // DIB Header
    set_bytes(dib_header, 0, 4, DIB_HEADER_SIZE); // DIB header size
    set_bytes(dib_header, 4, 4, width_pixels);    // Width of bitmap in pixels
    set_bytes(dib_header, 8, 4, height_pixels);   // Height of bitmap in pixels
    set_bytes(dib_header, 12, 2, 1);              // Number of color planes
    set_bytes(dib_header, 14, 2, 24);             // Number of bits per pixel
    set_bytes(dib_header, 16, 4, 0);              // Compression method (0=BI_RGB)
    set_bytes(dib_header, 20, 4, array_bytes);    // Size of raw bitmap data (including padding)
    set_bytes(dib_header, 24, 4, 2835);           // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 28, 4, 2835);           // Print resolution of image (2835 pixels/meter)
    set_bytes(dib_header, 32, 4, 0);              // Number of colors in palette
    set_bytes(dib_header, 36, 4, 0);              // Number of important colors

    // Write the BMP and DIB Headers to the file
    stream.write((char *)bmp_header, sizeof(bmp_header));
    stream.write((char *)dib_header, sizeof(dib_header));

    // Initialize pixel and padding
    unsigned char pixel[3] = {0};
    unsigned char padding[3] = {0};

    // Pixel Array (Left to right, bottom to top, with padding)
    for (int h = height_pixels - 1; h >= 0; h--)
    {
        for (int w = 0; w < width_pixels; w++)
        {
            // Write the pixel (Blue, Green, Red)
            pixel[0] = image[h][w].blue;
            pixel[1] = image[h][w].green;
            pixel[2] = image[h][w].red;
            stream.write((char *)pixel, 3);
        }
        // Write the padding bytes
        stream.write((char *)padding, padding_bytes);
    }

    // Close the stream and return true
    stream.close();
    return true;
}

//***************************************************************************************************//
//                                DO NOT MODIFY THE SECTION ABOVE                                    //
//***************************************************************************************************//
#include <iostream>
#include <limits>
#include <string>

namespace cli_utils
{

/**
 * Displays the main image processing menu to the console.
 *
 * Prints a formatted menu showing all available image processing options
 * (numbered 0-10) along with the currently selected image filename.
 * The menu prompts the user to enter a selection or 'Q' to quit.
 *
 * @param current_filename The name of the currently selected image file,
 *                         displayed in the menu header.
 */
void print_menu(const string &current_filename)
{
    cout << endl;
    cout << "IMAGE PROCESSING MENU" << endl;
    cout << "0) Change image (current: " << current_filename << ")" << endl;
    cout << "1) Vignette" << endl;
    cout << "2) Clarendon" << endl;
    cout << "3) Grayscale" << endl;
    cout << "4) Rotate 90 degrees" << endl;
    cout << "5) Rotate multiple 90 degrees" << endl;
    cout << "6) Enlarge" << endl;
    cout << "7) High contrast" << endl;
    cout << "8) Lighten" << endl;
    cout << "9) Darken" << endl;
    cout << "10) Black, white, red, green, blue" << endl;
    cout << "11) Rotate by arbitrary angle" << endl;
    cout << endl;
    cout << "Enter menu selection (Q to quit): ";
}

/**
 * Prompts the user for a BMP filename and returns the input.
 *
 * Displays the provided prompt message and reads a line of input from
 * standard input. The function does not validate the filename format
 * or check if the file exists.
 *
 * @param prompt_message The message to display when prompting for the filename.
 * @return The filename string entered by the user.
 */
string prompt_filename(const string &prompt_message)
{
    string filename;
    cout << prompt_message;
    getline(cin, filename);
    return filename;
}

/**
 * Prompts the user for an integer value, repeating until valid input is provided.
 *
 * Displays the prompt message and attempts to read an integer from standard input.
 * If the input is invalid (non-numeric or out of range), the function clears the
 * error state, discards the invalid input, displays an error message, and prompts
 * again. This continues until a valid integer is entered.
 *
 * @param prompt_message The message to display when prompting for the integer.
 * @return The integer value entered by the user.
 */
int prompt_int(const string &prompt_message)
{
    int value;
    while (true)
    {
        cout << prompt_message;
        cin >> value;
        if (!cin.fail())
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        else
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter an integer." << endl;
        }
    }
}

/**
 * Prompts the user for an integer value with a default, repeating until valid input is provided.
 *
 * Displays the prompt message and attempts to read an integer from standard input.
 * If the input is invalid (non-numeric or out of range), the function clears the
 * error state, discards the invalid input, displays an error message, and prompts
 * again. This continues until a valid integer is entered.
 *
 * Note: The default_value parameter is currently not used in the implementation,
 * but may be intended for future enhancement to allow users to press Enter to accept
 * the default value.
 *
 * @param prompt_message The message to display when prompting for the integer.
 * @param default_value The default integer value (currently unused in implementation).
 * @return The integer value entered by the user.
 */
int prompt_int(const string &prompt_message, int default_value)
{
    int value;
    while (true)
    {
        cout << prompt_message;
        cin >> value;
        if (!cin.fail())
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        else
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter an integer." << endl;
        }
    }
}

/**
 * Prompts the user for a double (floating-point) value, repeating until valid input is provided.
 *
 * Displays the prompt message and attempts to read a double from standard input.
 * If the input is invalid (non-numeric or out of range), the function clears the
 * error state, discards the invalid input, displays an error message, and prompts
 * again. This continues until a valid double is entered.
 *
 * @param prompt_message The message to display when prompting for the double value.
 * @return The double value entered by the user.
 */
double prompt_double(const string &prompt_message)
{
    double value;
    while (true)
    {
        cout << prompt_message;
        cin >> value;
        if (!cin.fail())
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        else
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a numeric value." << endl;
        }
    }
}

/**
 * Prompts the user for a double value within a specified range, repeating until valid input is provided.
 *
 * Displays the prompt message and attempts to read a double from standard input.
 * The input must be both a valid numeric value and within the specified [min_value, max_value] range.
 * If the input is invalid or out of range, the function clears the error state,
 * discards the invalid input, displays an error message with the valid range, and prompts
 * again. This continues until a valid double within range is entered.
 *
 * @param prompt_message The message to display when prompting for the double value.
 * @param min_value The minimum allowed value (inclusive).
 * @param max_value The maximum allowed value (inclusive).
 * @return The double value entered by the user, guaranteed to be within [min_value, max_value].
 */
double prompt_double(const string &prompt_message, double min_value, double max_value)
{
    double value;
    while (true)
    {
        cout << prompt_message;
        cin >> value;
        if (!cin.fail() && value >= min_value && value <= max_value)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        else
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a numeric value between " << min_value << " and " << max_value << "."
                 << endl;
        }
    }
}

/**
 * Pauses program execution and waits for the user to press Enter.
 *
 * Displays a "Press Enter to continue..." message and blocks until the user
 * presses Enter. This is useful for allowing users to read output before
 * the program continues or clears the screen.
 */
void wait_for_user()
{
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

/**
 * Prints a success message to standard output.
 *
 * Formats and displays a success message by prepending "Successfully " to
 * the provided message and appending "!". The output is sent to standard output (cout).
 *
 * @param message The message describing what operation was successful.
 */
void print_success(const string &message)
{
    cout << "Successfully " << message << "!" << endl;
}

/**
 * Prints an error message to standard error output.
 *
 * Formats and displays an error message by prepending "Error: " to the
 * provided message. The output is sent to standard error (cerr), which
 * allows error messages to be redirected separately from normal output.
 *
 * @param message The error message to display.
 */
void print_error(const string &message)
{
    cerr << "Error: " << message << endl;
}

/**
 * Prints the exit message when the program terminates.
 *
 * Displays a friendly farewell message to the user indicating that the
 * program is quitting. This is typically called when the user chooses
 * to exit the application.
 */
void print_exit_message()
{
    cout << "Thank you for using my program!" << endl;
    cout << "Quitting..." << endl;
}

} // namespace cli_utils

/**
 * @namespace image_processing
 * @brief Contains functions for applying various image processing filters and effects.
 *
 * The image_processing namespace provides a collection of functions that take an input image
 * (represented as a 2D vector of Pixel structs) and return a new image with different operations applied.
 * These operations include effects such as vignetting, color filters, rotations, resizing, grayscale,
 * high contrast, lightening, darkening, and posterization to primary colors or black/white.
 *
 * Each function is self-contained, does not modify its input, and returns a new processed image.
 */
namespace image_processing
{
/**
 * Applies a vignette effect to the input image.
 *
 * For each pixel, its color values are scaled down based on its distance from the
 * image center, creating a darkening effect toward the corners.
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @return A new image as a 2D vector of Pixels with the vignette effect applied.
 */
vector<vector<Pixel>> process_1(const vector<vector<Pixel>> &image)
{
    int height = image.size();
    if (height == 0)
        return {};
    int width = image[0].size();

    vector<vector<Pixel>> new_image(height, vector<Pixel>(width));
    double center_x = width / 2.0;
    double center_y = height / 2.0;

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            const Pixel &p = image[row][col];
            // Find the distance to the center
            double dx = col - center_x;
            double dy = row - center_y;
            double distance = sqrt(dx * dx + dy * dy);
            double scaling_factor = (height - distance) / height;
            if (scaling_factor < 0)
                scaling_factor = 0; // Avoid negative values for extreme corners

            Pixel new_pixel;
            new_pixel.red = static_cast<int>(p.red * scaling_factor);
            new_pixel.green = static_cast<int>(p.green * scaling_factor);
            new_pixel.blue = static_cast<int>(p.blue * scaling_factor);

            // Clamp values just in case (0-255)
            new_pixel.red = max(0, min(255, new_pixel.red));
            new_pixel.green = max(0, min(255, new_pixel.green));
            new_pixel.blue = max(0, min(255, new_pixel.blue));

            new_image[row][col] = new_pixel;
        }
    }
    return new_image;
}

/**
 * Applies the Clarendon filter effect to the input image.
 *
 * This effect increases contrast in the image by applying a scaling factor to
 * the pixel color values depending on their brightness:
 *   - For bright pixels (average >= 170), RGB values are increased toward 255.
 *   - For dark pixels (average < 90), RGB values are reduced by the scaling factor.
 *   - For mid-tone pixels (average between 90 and 170), original values are preserved.
 *
 * The scaling_factor parameter controls the strength of the effect (e.g., typical values: 0.5, 0.7).
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @param scaling_factor How strongly to adjust light and dark pixels; should be in [0, 1].
 * @return A new image as a 2D vector of Pixels with the Clarendon effect applied.
 */
vector<vector<Pixel>> process_2(const vector<vector<Pixel>> &image, double scaling_factor)
{
    int height = image.size();
    if (height == 0)
        return {};
    int width = image[0].size();

    vector<vector<Pixel>> new_image(height, vector<Pixel>(width));

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            const Pixel &p = image[row][col];
            int red_value = p.red;
            int green_value = p.green;
            int blue_value = p.blue;
            // average those values
            double average_value = (red_value + green_value + blue_value) / 3.0;

            int newred, newgreen, newblue;
            if (average_value >= 170)
            {
                newred = static_cast<int>(255 - (255 - red_value) * scaling_factor);
                newgreen = static_cast<int>(255 - (255 - green_value) * scaling_factor);
                newblue = static_cast<int>(255 - (255 - blue_value) * scaling_factor);
            }
            else if (average_value < 90)
            {
                newred = static_cast<int>(red_value * scaling_factor);
                newgreen = static_cast<int>(green_value * scaling_factor);
                newblue = static_cast<int>(blue_value * scaling_factor);
            }
            else
            {
                newred = red_value;
                newgreen = green_value;
                newblue = blue_value;
            }
            // Clamp values for safety
            newred = max(0, min(255, newred));
            newgreen = max(0, min(255, newgreen));
            newblue = max(0, min(255, newblue));

            Pixel new_pixel;
            new_pixel.red = newred;
            new_pixel.green = newgreen;
            new_pixel.blue = newblue;

            new_image[row][col] = new_pixel;
        }
    }
    return new_image;
}

/**
 * Applies a grayscale filter to the input image by averaging the red, green, and blue
 * color values of each pixel. The resulting image consists of pixels where all three
 * color channels are set to this average, producing a grayscale effect.
 *
 * @param image The input image represented as a 2D vector of Pixel structs.
 * @return A new 2D vector of Pixels where each pixel is the grayscale equivalent of the original.
 */
vector<vector<Pixel>> process_3(const vector<vector<Pixel>> &image)
{
    int height = image.size();
    if (height == 0)
        return {};
    int width = image[0].size();

    vector<vector<Pixel>> new_image(height, vector<Pixel>(width));

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            const Pixel &p = image[row][col];
            int red_value = p.red;
            int green_value = p.green;
            int blue_value = p.blue;

            // Calculate gray value as the average of the RGB components
            int gray_value = static_cast<int>((red_value + green_value + blue_value) / 3.0 + 0.5);

            // Set new pixel to grayscale
            Pixel new_pixel;
            new_pixel.red = gray_value;
            new_pixel.green = gray_value;
            new_pixel.blue = gray_value;

            new_image[row][col] = new_pixel;
        }
    }
    return new_image;
}

/**
 * Rotates the input image 90 degrees clockwise.
 *
 * The output image will have its dimensions transposed: if the input
 * image is height x width, the output will be width x height. Each pixel in the
 * original image at position (row, col) is moved to position (col, height-1-row)
 * in the rotated image.
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @return A new image as a 2D vector of Pixels, rotated 90 degrees clockwise.
 */
vector<vector<Pixel>> process_4(const vector<vector<Pixel>> &image)
{
    int height = image.size();
    if (height == 0)
        return {};
    int width = image[0].size();

    // Rotate 90 degrees clockwise: output is [width][height]
    vector<vector<Pixel>> new_image(width, vector<Pixel>(height));

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            // Place pixel at new position: (height-1-row, col)
            new_image[col][height - 1 - row] = image[row][col];
        }
    }
    return new_image;
}
/**
 * Rotates the input image by a multiple of 90 degrees clockwise.
 *
 * The image is rotated 'number' times by 90 degrees. Negative numbers will rotate counterclockwise.
 * Each 90-degree rotation clockwise transposes the image and reverses the order of its rows.
 *
 * For example:
 *  - number = 1 rotates 90 degrees clockwise,
 *  - number = 2 rotates 180 degrees,
 *  - number = 3 rotates 270 degrees clockwise (or 90 degrees counterclockwise),
 *  - number = 4 (or any multiple of 4) results in the same orientation as the original image.
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @param number The number of times to rotate the image by 90 degrees clockwise (can be negative).
 * @return A new image as a 2D vector of Pixels, rotated accordingly.
 */
vector<vector<Pixel>> process_5(const vector<vector<Pixel>> &image, int number)
{
    // process_5: Rotate image clockwise by (number * 90) degrees
    int angle = number * 90;
    // Normalize number of 90-degree rotations (clockwise)
    int rotations = ((number % 4) + 4) % 4; // Handles negative and >4

    vector<vector<Pixel>> rotated = image;

    for (int i = 0; i < rotations; ++i)
    {
        int h = rotated.size();
        int w = h ? rotated[0].size() : 0;
        vector<vector<Pixel>> temp(w, vector<Pixel>(h));
        for (int r = 0; r < h; ++r)
        {
            for (int c = 0; c < w; ++c)
            {
                temp[c][h - 1 - r] = rotated[r][c];
            }
        }
        rotated = std::move(temp);
    }
    return rotated;
}

/**
 * Enlarges the input image by the specified x and y scaling factors.
 *
 * Each pixel in the original image is multiplied into a block of size
 * x_scale by y_scale in the resulting image, producing a "pixelated"
 * enlargement effect.
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @param x_scale The scale factor for the width (columns); must be > 0.
 * @param y_scale The scale factor for the height (rows); must be > 0.
 * @return A new image as a 2D vector of Pixels with enlarged dimensions, or an
 *         empty vector if the scale factors or input are invalid.
 */
vector<vector<Pixel>> process_6(const vector<vector<Pixel>> &image, int x_scale, int y_scale)
{
    int height = image.size();
    if (height == 0 || x_scale <= 0 || y_scale <= 0)
        return {};

    int width = image[0].size();
    int new_width = x_scale * width;
    int new_height = y_scale * height;

    vector<vector<Pixel>> new_image(new_height, vector<Pixel>(new_width));

    // Iterate through the enlarged image
    for (int row = 0; row < new_height; ++row)
    {
        for (int col = 0; col < new_width; ++col)
        {
            // Map each pixel in the enlarged image back to the original image
            int orig_col = col / x_scale;
            int orig_row = row / y_scale;
            new_image[row][col] = image[orig_row][orig_col];
        }
    }
    return new_image;
}

/**
 * Converts the input image to high contrast (pure black and white).
 *
 * This function processes each pixel and sets it to either black (0,0,0)
 * or white (255,255,255), depending on whether the average brightness of
 * the pixel is below or above a threshold (128).
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @return A new image as a 2D vector of Pixels in high contrast (black and white).
 */
vector<vector<Pixel>> process_7(const vector<vector<Pixel>> &image)
{
    // process_7: Convert image to high contrast (black and white only)
    int height = image.size();
    if (height == 0)
        return {};
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel>(width));

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            const Pixel &p = image[row][col];
            int red_value = p.red;
            int green_value = p.green;
            int blue_value = p.blue;

            int gray_value = (red_value + green_value + blue_value) / 3;

            int newred, newgreen, newblue;
            if (gray_value >= 128)
            {
                newred = 255;
                newgreen = 255;
                newblue = 255;
            }
            else
            {
                newred = 0;
                newgreen = 0;
                newblue = 0;
            }

            new_image[row][col] = Pixel{newred, newgreen, newblue};
        }
    }
    return new_image;
}

/**
 * Lightens the input image by a given scaling factor.
 *
 * For each pixel, this function increases the brightness by moving each color channel
 * (red, green, blue) toward 255 (white) according to the provided scaling_factor.
 * The scaling_factor should be >= 0, where:
 *   - Values closer to 1.0 result in a lighter image (values approach 255)
 *   - Values closer to 0.0 result in less lightening (values stay closer to original)
 *
 * The formula for each new color channel is:
 *   new_value = 255 - (255 - original_value) * scaling_factor
 *
 * This operation moves pixel values toward white, producing a lighter image.
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @param scaling_factor Value >= 0 that controls how much to lighten each pixel.
 * @return A new image as a 2D vector of Pixels lightened by the scaling factor.
 */
vector<vector<Pixel>> process_8(const vector<vector<Pixel>> &image, double scaling_factor)
{
    // process_8: Lighten by a scaling factor
    int height = image.size();
    if (height == 0)
        return {};
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel>(width));

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            const Pixel &p = image[row][col];
            int red_value = p.red;
            int green_value = p.green;
            int blue_value = p.blue;

            // Lighten: use formula 255 - (255 - value) * scaling_factor
            int newred = static_cast<int>(255 - (255 - red_value) * scaling_factor);
            int newgreen = static_cast<int>(255 - (255 - green_value) * scaling_factor);
            int newblue = static_cast<int>(255 - (255 - blue_value) * scaling_factor);

            // Clamp values to [0, 255]
            newred = max(0, min(255, newred));
            newgreen = max(0, min(255, newgreen));
            newblue = max(0, min(255, newblue));

            new_image[row][col] = Pixel{newred, newgreen, newblue};
        }
    }
    return new_image;
}

/**
 * Darkens the input image by a given scaling factor.
 *
 * For each pixel, this function reduces the brightness by multiplying each color channel
 * (red, green, blue) by the provided scaling_factor.
 * The scaling_factor should be >= 0, where:
 *   - Values < 1.0 result in a darker image (values approach 0)
 *   - Values closer to 1.0 result in less darkening (values stay closer to original)
 *   - Value = 1.0 keeps the original brightness
 *
 * The formula for each new color channel is:
 *   new_value = original_value * scaling_factor
 *
 * This operation uniformly decreases the intensity of all color channels when factor < 1,
 * producing a darker image.
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @param scaling_factor Value >= 0 that controls how much to darken each pixel.
 * @return A new image as a 2D vector of Pixels darkened by the scaling factor.
 */
vector<vector<Pixel>> process_9(const vector<vector<Pixel>> &image, double scaling_factor)
{
    // process_9: Darken by a scaling factor
    int height = image.size();
    if (height == 0)
        return {};
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel>(width));

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            const Pixel &p = image[row][col];
            int red_value = p.red;
            int green_value = p.green;
            int blue_value = p.blue;

            // Darken: multiply by scaling_factor
            int newred = static_cast<int>(red_value * scaling_factor);
            int newgreen = static_cast<int>(green_value * scaling_factor);
            int newblue = static_cast<int>(blue_value * scaling_factor);

            // Clamp values to [0, 255]
            newred = max(0, min(255, newred));
            newgreen = max(0, min(255, newgreen));
            newblue = max(0, min(255, newblue));

            new_image[row][col] = Pixel{newred, newgreen, newblue};
        }
    }
    return new_image;
}

/**
 * Applies a filter that reduces each pixel's color to one of five options: pure red, pure green,
 * pure blue, white, or black.
 *
 * For each pixel:
 *   - If the sum of RGB channels is >= 550, the pixel is set to white (255,255,255).
 *   - If the sum of RGB channels is <= 150, the pixel is set to black (0,0,0).
 *   - Otherwise, the pixel is set to the primary color channel (red, green, or blue) with the
 *     highest value, as follows:
 *       - If red is the highest, the pixel becomes pure red (255,0,0).
 *       - If green is the highest, the pixel becomes pure green (0,255,0).
 *       - Otherwise, the pixel becomes pure blue (0,0,255).
 *
 * This creates a posterized, high-contrast effect that simplifies the original image into
 * a small set of bold color regions.
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @return A new image as a 2D vector of Pixels with the filter applied.
 */
vector<vector<Pixel>> process_10(const vector<vector<Pixel>> &image)
{
    // process_10: Filter to limited color channels: Red, Blue, Green, White, Black
    int height = image.size();
    if (height == 0)
        return {};
    int width = image[0].size();
    vector<vector<Pixel>> new_image(height, vector<Pixel>(width));

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            const Pixel &p = image[row][col];
            int red_value = p.red;
            int green_value = p.green;
            int blue_value = p.blue;
            int sum = red_value + green_value + blue_value;
            int max_color = max({red_value, green_value, blue_value});

            int newred = 0, newgreen = 0, newblue = 0;

            if (sum >= 550)
            { // White
                newred = 255;
                newgreen = 255;
                newblue = 255;
            }
            else if (sum <= 150)
            { // Black
                newred = 0;
                newgreen = 0;
                newblue = 0;
            }
            else if (max_color == red_value)
            { // Red
                newred = 255;
                newgreen = 0;
                newblue = 0;
            }
            else if (max_color == green_value)
            { // Green
                newred = 0;
                newgreen = 255;
                newblue = 0;
            }
            else
            { // Blue
                newred = 0;
                newgreen = 0;
                newblue = 255;
            }

            new_image[row][col] = Pixel{newred, newgreen, newblue};
        }
    }
    return new_image;
}

/**
 * Rotates the input image by an arbitrary angle (1-359 degrees) clockwise.
 *
 * This function rotates the image around its center by the specified angle.
 * The output image will have dimensions large enough to contain the entire
 * rotated image (bounding box). Pixels in the output image that don't map
 * to valid source pixels are set to black (0,0,0).
 *
 * The rotation uses bilinear interpolation to determine pixel values when
 * the inverse rotation maps to non-integer coordinates in the source image.
 *
 * @param image The input image as a 2D vector of Pixels (row-major order).
 * @param degrees The angle in degrees (1-359) to rotate clockwise.
 * @return A new image as a 2D vector of Pixels, rotated by the specified angle.
 */
vector<vector<Pixel>> process_11(const vector<vector<Pixel>> &image, int degrees)
{
    int height = image.size();
    if (height == 0)
        return {};
    int width = image[0].size();

    // Convert degrees to radians (clockwise rotation, so use negative)
    double angle_rad = -degrees * M_PI / 180.0;
    double cos_angle = cos(angle_rad);
    double sin_angle = sin(angle_rad);

    // Calculate the center of the original image
    double center_x = width / 2.0;
    double center_y = height / 2.0;

    // Calculate bounding box dimensions for rotated image
    // Corners of the original image
    double corners[4][2] = {
        {0 - center_x, 0 - center_y},          // top-left
        {width - center_x, 0 - center_y},      // top-right
        {width - center_x, height - center_y}, // bottom-right
        {0 - center_x, height - center_y}      // bottom-left
    };

    // Rotate corners and find min/max
    double min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    for (int i = 0; i < 4; i++)
    {
        double x = corners[i][0];
        double y = corners[i][1];
        double rot_x = x * cos_angle - y * sin_angle;
        double rot_y = x * sin_angle + y * cos_angle;
        if (i == 0 || rot_x < min_x)
            min_x = rot_x;
        if (i == 0 || rot_x > max_x)
            max_x = rot_x;
        if (i == 0 || rot_y < min_y)
            min_y = rot_y;
        if (i == 0 || rot_y > max_y)
            max_y = rot_y;
    }

    // Calculate new image dimensions
    // Add 1 to ensure we have enough pixels to cover the entire rotated image
    int new_width = static_cast<int>(ceil(max_x - min_x)) + 1;
    int new_height = static_cast<int>(ceil(max_y - min_y)) + 1;
    double new_center_x = new_width / 2.0;
    double new_center_y = new_height / 2.0;

    // Create output image
    vector<vector<Pixel>> new_image(new_height, vector<Pixel>(new_width));

    // Inverse rotation matrix (to map output pixels back to input)
    // For clockwise rotation by θ, inverse is counterclockwise by θ
    double inv_cos = cos(-angle_rad);
    double inv_sin = sin(-angle_rad);

    // For each pixel in the output image
    for (int row = 0; row < new_height; ++row)
    {
        for (int col = 0; col < new_width; ++col)
        {
            // Convert to coordinates relative to new center
            double x = col - new_center_x;
            double y = row - new_center_y;

            // Apply inverse rotation to get coordinates in original image
            double orig_x = x * inv_cos - y * inv_sin + center_x;
            double orig_y = x * inv_sin + y * inv_cos + center_y;

            // Bilinear interpolation
            int x0 = static_cast<int>(floor(orig_x));
            int y0 = static_cast<int>(floor(orig_y));
            int x1 = x0 + 1;
            int y1 = y0 + 1;

            // Check bounds
            if (x0 < 0 || y0 < 0 || x1 >= width || y1 >= height)
            {
                // Out of bounds - set to black
                new_image[row][col] = Pixel{0, 0, 0};
                continue;
            }

            // Calculate interpolation weights
            double dx = orig_x - x0;
            double dy = orig_y - y0;

            // Get pixel values at four corners
            Pixel p00 = image[y0][x0];
            Pixel p10 = image[y0][x1];
            Pixel p01 = image[y1][x0];
            Pixel p11 = image[y1][x1];

            // Bilinear interpolation for each color channel
            double red =
                (1 - dx) * (1 - dy) * p00.red + dx * (1 - dy) * p10.red + (1 - dx) * dy * p01.red + dx * dy * p11.red;

            double green = (1 - dx) * (1 - dy) * p00.green + dx * (1 - dy) * p10.green + (1 - dx) * dy * p01.green +
                           dx * dy * p11.green;

            double blue = (1 - dx) * (1 - dy) * p00.blue + dx * (1 - dy) * p10.blue + (1 - dx) * dy * p01.blue +
                          dx * dy * p11.blue;

            // Round and clamp values
            int new_red = max(0, min(255, static_cast<int>(round(red))));
            int new_green = max(0, min(255, static_cast<int>(round(green))));
            int new_blue = max(0, min(255, static_cast<int>(round(blue))));

            new_image[row][col] = Pixel{new_red, new_green, new_blue};
        }
    }

    return new_image;
}

} // namespace image_processing

//***************************************************************************************************//
//                                MAIN FUNCTION                                                      //
//***************************************************************************************************//

int main()
{
    string current_filename = "";
    bool running = true;
    while (running)
    {
        cli_utils::print_menu(current_filename);

        string selection;
        getline(cin, selection);

        // Handle quit (accepts lowercase and uppercase Q)
        if (selection == "Q" || selection == "q")
        {
            cli_utils::print_exit_message();
            break;
        }

        // Handle Menu Selection and Actions
        try
        {
            int sel_num = stoi(selection);

            // Handle 0 (Change image)
            if (sel_num == 0)
            {
                string filename = cli_utils::prompt_filename("Enter new input BMP filename: ");
                // Check if the filename ends with ".bmp" (case-insensitive), if not, append ".bmp"
                string lower_filename = filename;
                // Convert to lowercase for checking extension
                transform(lower_filename.begin(), lower_filename.end(), lower_filename.begin(),
                          [](unsigned char c) { return tolower(c); });
                if (lower_filename.size() < 4 || lower_filename.substr(lower_filename.size() - 4) != ".bmp")
                {
                    filename += ".bmp";
                }
                current_filename = filename;
                cli_utils::print_success("changed input image");
            }

            // Handle 1-11 (image processing and output)
            else if (sel_num >= 1 && sel_num <= 11)
            {
                if (current_filename.empty())
                {
                    cli_utils::print_error("No input image selected. Please select an image first.");
                }
                else
                {
                    auto image = read_image(current_filename);
                    if (image.empty())
                    {
                        cli_utils::print_error("Failed to open or read the image file: " + current_filename);
                        cli_utils::print_error("Check your filepath points to a valid .bmp image file, and try again.");
                        continue; // Go back to the main menu so the user can re-enter the filename
                    }
                    else
                    {
                        vector<vector<Pixel>> result;
                        switch (sel_num)
                        {
                        case 1:
                            // Vignette; no extra input.
                            result = image_processing::process_1(image);
                            break;
                        case 2: {
                            // Clarendon; ask for scaling factor
                            double scaling_factor = cli_utils::prompt_double(
                                "Enter a scaling factor (0.0 - 1.0) for Clarendon: ", 0.0, 1.0);
                            result = image_processing::process_2(image, scaling_factor);
                            break;
                        }
                        case 3:
                            // Grayscale; no extra input
                            result = image_processing::process_3(image);
                            break;
                        case 4:
                            // Rotate 90 degrees clockwise; no extra input
                            result = image_processing::process_4(image);
                            break;
                        case 5: {
                            // Rotate by user-specified multiple of 90 degrees
                            int number = cli_utils::prompt_int(
                                "Enter number of times to rotate (90 degrees each, can be negative): ");
                            result = image_processing::process_5(image, number);
                            break;
                        }
                        case 6: {
                            // Resize; prompt for x and y scale
                            int x_scale = cli_utils::prompt_int("Enter scale factor for width (x_scale > 0): ", 1);
                            int y_scale = cli_utils::prompt_int("Enter scale factor for height (y_scale > 0): ", 1);
                            result = image_processing::process_6(image, x_scale, y_scale);
                            break;
                        }
                        case 7:
                            // High contrast (black and white)
                            result = image_processing::process_7(image);
                            break;
                        case 8: {
                            // Lighten; prompt for scaling factor
                            double scaling_factor =
                                cli_utils::prompt_double("Enter a scaling factor (>= 0.0) for lightening: ", 0.0, 10.0);
                            result = image_processing::process_8(image, scaling_factor);
                            break;
                        }
                        case 9: {
                            // Darken; prompt for scaling factor
                            double scaling_factor =
                                cli_utils::prompt_double("Enter a scaling factor (>= 0.0) for darkening: ", 0.0, 10.0);
                            result = image_processing::process_9(image, scaling_factor);
                            break;
                        }
                        case 10:
                            // Primary channel/posterize (red/green/blue/white/black)
                            result = image_processing::process_10(image);
                            break;
                        case 11: {
                            // Rotate by arbitrary angle (1-359 degrees)
                            int degrees;
                            while (true)
                            {
                                degrees = cli_utils::prompt_int("Enter rotation angle in degrees (1-359): ");
                                if (degrees >= 1 && degrees <= 359)
                                {
                                    break;
                                }
                                else
                                {
                                    cli_utils::print_error("Angle must be between 1 and 359 degrees.");
                                }
                            }
                            result = image_processing::process_11(image, degrees);
                            break;
                        }
                        default:
                            cli_utils::print_error("Unknown processing selection.");
                            break;
                        }

                        // Save the processed image if a result was generated
                        if (!result.empty())
                        {
                            string out_filename =
                                cli_utils::prompt_filename("Enter output filename for the processed BMP image: ");
                            // Ensure the filename ends with .bmp (case-insensitive)
                            if (out_filename.length() < 4 ||
                                !(out_filename.size() >= 4 &&
                                  ((out_filename.substr(out_filename.size() - 4) == ".bmp") ||
                                   (out_filename.substr(out_filename.size() - 4) == ".BMP"))))
                            {
                                out_filename += ".bmp";
                            }
                            if (write_image(out_filename, result))
                            {
                                cli_utils::print_success("output image written: " + out_filename);
                            }
                            else
                            {
                                cli_utils::print_error("Failed to write output image: " + out_filename);
                            }
                        }
                    }
                }
            }
            else
            {
                cli_utils::print_error("Invalid menu selection");
            }
        }
        catch (...)
        {
            cli_utils::print_error("Invalid input");
        }
        // Ask user to hit enter to advance ALWAYS
        cli_utils::wait_for_user();
    }
    return 0;
}