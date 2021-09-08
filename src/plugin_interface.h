// https://www.cosmigo.com/pixel_animation_software/plugins/developer-interface

#ifdef __cplusplus
extern "C" {
#endif

typedef void (__stdcall *ProgressCallback)( int progress );

/**
 * General initialization function.
 * It is called once the plugin is registered by the application for further use.
 * It is the first function that will be called at all.
 * If an error occurs then the function must return false.
 *
 * @param language Output - two character ISO language code that is currently
 * used to display translated messages.
 * @param version Output - the number of the plugin interface version.
 * @param animation Output - set to true if the plugin is used for accessing
 * animation files.
 * @return true if successful, false otherwise.
 */
bool __stdcall initialize(
	char* language, unsigned short* version, bool* animation
);

/**
 * Use this function to define a progress callback that is called by the plugin
 * to give user feedback about the progress of loading/saving image data.
 * A percent value of 0 should make the progress display to hide and values
 * between 1 and 100 should make the display visible.
 *
 * @param progressCallback A function to be called when progress changes.
 */
void __stdcall setProgressCallback( ProgressCallback progressCallback );

/**
 * Typically only certain functions are allowed to set an error message to not
 * exaggerate error handling.
 * Right after executing those functions getErrorMessage(void) is called
 * by the application.
 * If there is an error message then the result of the failing function
 * is dismissed and the error is displayed.
 * The plugin must reset its internal error message after getErrorMessage(void)
 * was called.
 * Since the application can only reset the error by calling getErrorMessage(void)
 * every function could theoretically set an error.
 * But if it is not explicitly allowed then the error message is not checked
 * until a function is called that is allowed to set an error.
 * This function will fail then instead of the one setting the error.
 * The plugin should therefore only set errors when allowed.
 *
 * @return The error message or NULL.
 */
char* __stdcall getErrorMessage(void);

/**
 * The plugin must return a unique identifier that is a name/alias
 * for the plugin implementation.
 * It is used internally for example if the user saved the file via this plugin
 * and uses the "save again" function.
 * In this case the application must know which plugin to use.
 * The file extension is not unique enough.
 * There could be several load/save plugins for "bmp-files".
 * The id may be a series of numbers/characters like a GUID, but it may also be
 * a package descriptor like used in Java language,
 * e.g. "de.mycompany.promotion.ioplugin.png".
 *
 * @return The unique identifier.
 */
const char* __stdcall getFileTypeId(void);

/**
 * The application needs to know if the plugin can read the file format
 * to place it into the file open/import dialogs.
 *
 * @return True, if read is supported, false otherwise.
 */
bool __stdcall isReadSupported(void);

/**
 * The application needs to know if the plugin can write the file format
 * to place it into the file save/export dialogs.
 *
 * @return True, if write is supported, false otherwise.
 */
bool __stdcall isWriteSupported(void);

/**
 * The application needs to know if the plugin can write true color data
 * to the file format.
 * Certain processes like auto flattening layers may create colors that
 * don't fit into the 256 colors palette.
 * In this case the image data can be optionally stored as true color.
 * If the plugin does not support true color then the image colors are reduced
 * to 256 indexed colors.
 *
 * @return True, if write is supported, false otherwise.
 */
bool __stdcall isWriteTrueColorSupported(void);

/**
 * The application may support functions to load the color palette from
 * a graphic file without loading the graphic/bitmap data.
 * It uses this function to determine if the plugin can be used there as well.
 * In that case the plugin is added in dialogs used when loading color palettes.
 *
 * @return If the plugin supports palette reading then this function must
 * return true.
 */
bool __stdcall canExtractPalette(void);

/**
 * To place the plugin into file io dialogs it must give a file type
 * description that is displayed in the file filter box,
 * e.g. "BMP Windows Bitmap RLE".
 * Please use the file type abbreviation (usually the file extension)
 * at first place so that it can be sorted correctly.
 *
 * @return The file description in the selected language.
 */
char* __stdcall getFileBoxDescription(void);

/**
 * This function must return the file extension (without ".") to be used
 * in the file filter.
 *
 * @return The file extension supported by this plugin.
 */
char* __stdcall getFileExtension(void);

/**
 * Indicates that a new file is to be processed and gives the corresponding
 * file name.
 * The plugin should reset internal structures if the file name
 * is different to the one set before.
 * At this point it is undefined if the file is to be written or read!
 *
 * @param filename Full path and name of the file to process.
 */
void __stdcall setFilename( char* filename );

/**
 * This function is called to check if the selected file can be handled for
 * reading by the plugin. The plugin should open and check the file accordingly.
 *
 * @return True, if the file can be processed. If false is returned then
 * an error message must be set saying why it can not be handled.
 */
bool __stdcall canHandle(void);

/**
 * Before reading graphic data this function is called to make the plugin
 * check and load graphic file information such as dimensions, color palette
 * and the like.
 * Other functions rely on this function to be called first, such as GetWidth!
 *
 * @return True, if the file data could be loaded.
 */
bool __stdcall loadBasicData(void);

/**
 * Dimension request for width when loading the file.
 *
 * LoadBasicData() has been called by the application before using this function
 * to ensure that this information is present.
 *
 * @return The width of the image that is to be loaded in Pixels or -1
 * if the function fails.
 */
int __stdcall getWidth(void);

/**
 * Dimension request for height when loading the file.
 *
 * LoadBasicData() has been called by the application before using this function
 * to ensure that this information is present.
 *
 * @return The height of the image that is to be loaded in Pixels or -1
 * if the function fails.
 */
int __stdcall getHeight(void);

/**
 * This function has to return the number of frames available to load
 * from the file.
 * If the file is just a single image then “1” is to be returned.
 * LoadBasicData() has been called by the application before using this function
 * to ensure that this information is present.
 *
 * @return The number of frames of the image/animation that is to be loaded.
 */
int __stdcall getImageCount(void);

/**
 * If the plugin can extract the palette data then this function must return
 * the palette with 768 bytes defining the 256 color values as RGB
 * (one byte per channel).
 * The palette bytes are RGBRGBRGB... and each RGB-tripel defines
 * the corresponding color palette entry starting with “0”.
 *
 * LoadBasicData() has been called by the application before using this function
 * to ensure that this information is present. It is only called if
 * canExtractPalette returns true.
 *
 * @return The RGB palette or nil/NULL if not supported
 */
unsigned char* __stdcall getRgbPalette(void);

/**
 * If the image contains a transparent color then this function must return it.
 *
 * LoadBasicData() has been called by the application before using this function
 * to ensure that this information is present.
 *
 * @return The transparent color (pixel byte) or -1 if there is no transparent
 * color.
 */
int __stdcall getTransparentColor(void);

/**
 * Does the image/animation file contain alpha transparency data?
 *
 * LoadBasicData() has been called by the application before using this function
 * to ensure that this information is present.
 *
 * @return If the image contains alpha data then this function must return true.
 */
bool __stdcall isAlphaEnabled(void);

/**
 * If the plugin supports reading then this function is used to load
 * the image data.
 * After reading this data the plugin must advance to the next frame, if any.
 * The function will be called according to the number of frames returned
 * by GetImageCount.
 *
 * LoadBasicData() has been called by the application before using this function
 * to ensure that this information is present.
 *
 * @param colorFrame a pointer to the bitmap to hold the color pixels
 * (color palette indexes). The memory portion has a size of
 * GetWidth * GetHeight bytes!
 * @param colorFramePalette a pointer to the rgb color table.
 * There are 768 bytes being 256 colors with one byte for red, green and blue.
 * @param alphaFrame a pointer to the bitmap to hold the alpha palette indexes.
 * The memory portion has a size of GetWidth * GetHeight bytes! If alpha is not
 * supported then this value is nil/NULL and must not be used.
 * @param alphaFramePalette a pointer to the alpha value table.
 * There are 256 bytes. Each byte is an alpha value ranging from 0 to 255.
 * If alpha is not supported then this value is nil/NULL and must not be used.
 * @param delayMs if the frame has a delay value (animation only) then
 * it must be given here as milliseconds.
 * @return If the data was transfered successfuly then true is to be returned.
 */
bool __stdcall loadNextImage(
	unsigned char* colorFrame, unsigned char* colorFramePalette,
	unsigned char* alphaFrame, unsigned char* alphaFramePalette,
	unsigned short* delayMs
);

/**
 * Before writing graphic data this function is called once by the application
 * to define dimensions of the data that will be stored. The file may stay
 * opened until FinishProcessing is called.
 *
 * @param width width of the graphic (images).
 * @param height height of the graphic (images).
 * @param transparentColor if a transparent color is used then it is given here
 * or -1 otherwise.
 * @param alphaEnabled if the graphic will store alpha data then this flag
 * is set to true.
 * @param numberOfFrames number of frames that will be written.
 * @return true on success.
 */
bool __stdcall beginWrite(
	int width, int height, int transparentColor, bool alphaEnabled,
	int numberOfFrames
);

/**
 * @brief If the plugin supports writing then this function is used to save
 * the image data. The function will be called as often as there are more frames
 * to be stored.
 *
 * @param colorFrame a pointer to the bitmap having the color pixels
 * (color palette indexes). The memory portion has a size width * height bytes!
 * @param colorFramePalette a pointer to the rgb color table. There are 768
 * bytes being 256 colors with one byte for red, green and blue
 * @param alphaFrame a pointer to the bitmap having the alpha palette indexes.
 * The memory portion has a size of width * height bytes!
 * If alpha is not supported then this value is nil and must not be used.
 * @param alphaFramePalette a pointer to the alpha value table.
 * There are 256 bytes. Each byte is an alpha value ranging from 0 to 255.
 * If alpha is not supported then this value is nil/NULL and must not be used.
 * @param delayMs if the frame has a delay value (animation only) then
 * it is given here as milliseconds.
 * @return If the data was transfered successfuly then true is to be returned
 */
bool __stdcall writeNextImage(
	unsigned char* colorFrame, unsigned char* colorFramePalette,
	unsigned char* alphaFrame, unsigned char* alphaFramePalette,
	unsigned short delayMs
);

/**
 * This function is called if the read or write operation of image data
 * is finished.
 * Upon call the plugin must then close the file that was processed and do other
 * internally required clean up.
 * It is also called if there was an error during read and write.
 */
void __stdcall finishProcessing(void);

#ifdef __cplusplus
}
#endif

