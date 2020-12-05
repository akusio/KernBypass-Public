#import "CCUICAPackageDescription.h"
#import "CCUIContentModule-Protocol.h"

@interface CCUIToggleModule : NSObject <CCUIContentModule>
@property (assign, getter=isSelected, nonatomic) BOOL selected;
@property (nonatomic, copy, readonly) UIImage *iconGlyph;
@property (nonatomic, copy, readonly) UIImage *selectedIconGlyph;
@property (nonatomic, copy, readonly) UIColor *selectedColor;
@property (nonatomic, copy, readonly) CCUICAPackageDescription *glyphPackageDescription;
@property (nonatomic, readonly) UIViewController<CCUIContentModuleContentViewController> *contentViewController;
@property (nonatomic, readonly) UIViewController *backgroundViewController;
- (UIViewController<CCUIContentModuleContentViewController> *)contentViewController;

// For When the model is selected, refreshState is not called automagically;
- (BOOL)isSelected;
- (void)setSelected:(BOOL)selected;

- (void)refreshState; // Force a refresh of the switch state

/* 
 * If you're using an image as you icon gylph, Icon glyphs should have 
 * a height of 48px for @2x and 72 for @3x, the width may be whatever.
 */

- (UIColor *)selectedColor;
- (UIImage *)iconGlyph;
- (UIImage *)selectedIconGlyph; // if the selected should be different from the non-selected;

// If you're using a CAPackage for the icon glyph
- (NSString *)glyphState;
- (CCUICAPackageDescription *)glyphPackage;
@end