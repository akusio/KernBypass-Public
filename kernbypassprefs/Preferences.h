#import <QuartzCore/QuartzCore.h>
#import <objc/runtime.h>

typedef enum PSCellType {
    PSGroupCell,
    PSLinkCell,
    PSLinkListCell,
    PSListItemCell,
    PSTitleValueCell,
    PSSliderCell,
    PSSwitchCell,
    PSStaticTextCell,
    PSEditTextCell,
    PSSegmentCell,
    PSGiantIconCell,
    PSGiantCell,
    PSSecureEditTextCell,
    PSButtonCell,
    PSEditTextViewCell,
} PSCellType;


@interface UIImage (SettingsKit)
+ (UIImage *)imageNamed:(NSString *)named inBundle:(NSBundle *)bundle;
@end


@interface PSSwitchTableCell
{
}
@end

@interface PSTextEditingCell : UITableViewCell
{
}

- (void)layoutSubviews;

@end

@interface UIKeyboardCandidateView
+ (id)sharedInstanceForInlineView;
@end

@interface UIKeyboardLayout : UIView
- (id)initWithFrame:(struct CGRect)arg1;
- (void)setRenderConfig:(id)arg1;
@end

@interface UIKBRenderConfig : NSObject
+ (id)darkConfig;
+ (id)defaultConfig;
@end

@interface UIKeyboard : UIView
- (void)movedFromSuperview:(id)arg1;
- (_Bool)isActive;
+ (id)activeKeyboard;
- (id)initWithDefaultSize;
- (id)initWithFrame:(struct CGRect)arg1;
- (void)activate;
- (void)maximize;
- (void)setNeedsDisplay;
- (void)updateLayout;
+ (void)initImplementationNow;
- (void)_setRenderConfig:(id)arg1;
@end

@interface PSListController : UIViewController <UITableViewDelegate, UITableViewDataSource>{
    NSMutableDictionary *_cells;
    UITableView *_table;
    NSArray *_specifiers;
}

+ (BOOL)displaysButtonBar;

- (void)addSpecifier:(id)arg1;
- (id)popupStylePopoverController;
- (void)showPINSheet:(id)arg1;
- (id)specifierIDPendingPush;
- (id)pendingURLResourceDictionary;
- (BOOL)forceSynchronousIconLoadForCreatedCells;
- (id)specifierDataSource;
- (void)_setNotShowingSetupController;
- (BOOL)shouldReloadSpecifiersOnResume;
- (void)selectRowForSpecifier:(id)arg1;
- (BOOL)handlePendingURL;
- (void)dismissPopover;
- (id)specifiersForIDs:(id)arg1;
- (id)controllerForRowAtIndexPath:(id)arg1;
- (void)lazyLoadBundle:(id)arg1;
- (void)showConfirmationViewForSpecifier:(id)arg1 useAlert:(BOOL)arg2;
- (void)createPrequeuedPSTableCells:(unsigned long long)arg1;
- (id)cachedCellForSpecifierID:(id)arg1;
- (id)specifierID;
- (void)loseFocus;
- (void)updateSpecifiers:(id)arg1 withSpecifiers:(id)arg2;
- (void)replaceContiguousSpecifiers:(id)arg1 withSpecifiers:(id)arg2;
- (void)removeContiguousSpecifiers:(id)arg1;
- (void)removeLastSpecifier;
- (void)removeSpecifierAtIndex:(long long)arg1;
- (void)removeSpecifierID:(id)arg1;
- (void)removeSpecifier:(id)arg1;
- (void)addSpecifiersFromArray:(id)arg1;
- (void)insertContiguousSpecifiers:(id)arg1 atEndOfGroup:(long long)arg2;
- (void)insertContiguousSpecifiers:(id)arg1 afterSpecifierID:(id)arg2;
- (void)insertContiguousSpecifiers:(id)arg1 afterSpecifier:(id)arg2;
- (void)insertContiguousSpecifiers:(id)arg1 atIndex:(long long)arg2;
- (void)insertSpecifier:(id)arg1 atEndOfGroup:(long long)arg2;
- (void)insertSpecifier:(id)arg1 afterSpecifierID:(id)arg2;
- (void)insertSpecifier:(id)arg1 afterSpecifier:(id)arg2;
- (void)insertSpecifier:(id)arg1 atIndex:(long long)arg2;
- (long long)indexForRow:(long long)arg1 inGroup:(long long)arg2;
- (BOOL)getGroup:(long long*)arg1 row:(long long*)arg2 ofSpecifierID:(id)arg3;
- (long long)numberOfGroups;
- (BOOL)containsSpecifier:(id)arg1;
- (void)reloadSpecifierID:(id)arg1;
- (void)reloadSpecifierAtIndex:(long long)arg1;
- (void)setSpecifiers:(id)arg1;
- (long long)observerType;
- (void)invalidateSpecifiersForDataSource:(id)arg1;
- (void)dataSource:(id)arg1 performUpdates:(id)arg2;
- (void)_moveSpecifierAtIndex:(unsigned long long)arg1 toIndex:(unsigned long long)arg2 animated:(BOOL)arg3;
- (void)performSpecifierUpdates:(id)arg1;
- (void)_performHighlightForSpecifierWithID:(id)arg1;
- (long long)indexOfSpecifierID:(id)arg1;
- (void)reloadIconForSpecifierForBundle:(id)arg1;
- (BOOL)prepareHandlingURLForSpecifierID:(id)arg1 resourceDictionary:(id)arg2 animatePush:(BOOL*)arg3;
- (void)highlightSpecifierWithID:(id)arg1;
- (float)verticalContentOffset;
- (void)setDesiredVerticalContentOffsetItemNamed:(id)arg1;
- (void)setDesiredVerticalContentOffset:(float)arg1;
- (void)setSpecifierIDPendingPush:(id)arg1;
- (void)setPendingURLResourceDictionary:(id)arg1;
- (BOOL)shouldDeferPushForSpecifierID:(id)arg1;
- (void)showController:(id)arg1 animate:(BOOL)arg2;
- (void)setEdgeToEdgeCells:(BOOL)arg1;
- (void)showController:(id)arg1;
- (void)showConfirmationViewForSpecifier:(id)arg1;
- (id)indexPathForSpecifier:(id)arg1;
- (BOOL)performLoadActionForSpecifier:(id)arg1;
- (BOOL)performButtonActionForSpecifier:(id)arg1;
- (BOOL)performActionForSpecifier:(id)arg1;
- (id)controllerForSpecifier:(id)arg1;
- (void)_handleActionSheet:(id)arg1 clickedButtonAtIndex:(long long)arg2;
- (void)confirmationViewCancelledForSpecifier:(id)arg1;
- (void)confirmationViewAcceptedForSpecifier:(id)arg1;
- (BOOL)performConfirmationCancelActionForSpecifier:(id)arg1;
- (BOOL)performConfirmationActionForSpecifier:(id)arg1;
- (void)showConfirmationViewForSpecifier:(id)arg1 useAlert:(BOOL)arg2 swapAlertButtons:(BOOL)arg3;
- (void)returnPressedAtEnd;
- (void)popupViewWillDisappear;
- (void)formSheetViewWillDisappear;
- (void)_performHighlightForSpecifierWithID:(id)arg1 tryAgainIfFailed:(BOOL)arg2;
- (double)_getKeyboardIntersectionHeight;
- (void)_loadBundleControllers;
- (void)_scrollToSpecifierNamed:(id)arg1;
- (id)findFirstVisibleResponder;
- (BOOL)shouldSelectResponderOnAppearance;
- (id)_tableView:(id)arg1 viewForCustomInSection:(NSInteger)arg2 isHeader:(BOOL)arg3;
- (double)_tableView:(id)arg1 heightForCustomInSection:(NSInteger)arg2 isHeader:(BOOL)arg3;
- (id)_customViewForSpecifier:(id)arg1 class:(Class)arg2 isHeader:(BOOL)arg3;
- (id)cachedCellForSpecifier:(id)arg1;
- (void)setForceSynchronousIconLoadForCreatedCells:(BOOL)arg1;
- (void)migrateSpecifierMetadataFrom:(id)arg1 to:(id)arg2;
- (id)specifierAtIndex:(long long)arg1;
- (id)_createGroupIndices:(id)arg1;
- (Class)tableViewClass;
- (BOOL)_isRegularWidth;
- (BOOL)edgeToEdgeCells;
- (void)contentSizeChangedNotificationPosted:(id)arg1;
- (void)contentSizeDidChange:(id)arg1;
- (void)_returnKeyPressed:(id)arg1;
- (void)_unloadBundleControllers;
- (void)dismissConfirmationViewForSpecifier:(id)arg1 animated:(BOOL)arg2;
- (long long)_nextGroupInSpecifiersAfterIndex:(long long)arg1 inArray:(id)arg2;
- (void)replaceContiguousSpecifiers:(id)arg1 withSpecifiers:(id)arg2 animated:(BOOL)arg3;
- (void)_removeContiguousSpecifiers:(id)arg1 animated:(BOOL)arg2;
- (void)_removeIdentifierForSpecifier:(id)arg1;
- (void)removeLastSpecifierAnimated:(BOOL)arg1;
- (void)removeSpecifierAtIndex:(long long)arg1 animated:(BOOL)arg2;
- (void)removeSpecifierID:(id)arg1 animated:(BOOL)arg2;
- (void)removeSpecifier:(id)arg1 animated:(BOOL)arg2;
- (void)removeContiguousSpecifiers:(id)arg1 animated:(BOOL)arg2;
- (void)addSpecifiersFromArray:(id)arg1 animated:(BOOL)arg2;
- (void)addSpecifier:(id)arg1 animated:(BOOL)arg2;
- (void)insertContiguousSpecifiers:(id)arg1 atEndOfGroup:(long long)arg2 animated:(BOOL)arg3;
- (void)insertContiguousSpecifiers:(id)arg1 afterSpecifierID:(id)arg2 animated:(BOOL)arg3;
- (void)insertContiguousSpecifiers:(id)arg1 afterSpecifier:(id)arg2 animated:(BOOL)arg3;
- (void)_insertContiguousSpecifiers:(id)arg1 atIndex:(long long)arg2 animated:(BOOL)arg3;
- (void)_addIdentifierForSpecifier:(id)arg1;
- (void)insertSpecifier:(id)arg1 atEndOfGroup:(long long)arg2 animated:(BOOL)arg3;
- (void)insertSpecifier:(id)arg1 afterSpecifierID:(id)arg2 animated:(BOOL)arg3;
- (void)insertSpecifier:(id)arg1 afterSpecifier:(id)arg2 animated:(BOOL)arg3;
- (void)insertSpecifier:(id)arg1 atIndex:(long long)arg2 animated:(BOOL)arg3;
- (void)insertContiguousSpecifiers:(id)arg1 atIndex:(long long)arg2 animated:(BOOL)arg3;
- (long long)rowsForGroup:(long long)arg1;
- (BOOL)_getGroup:(long long*)arg1 row:(long long*)arg2 ofSpecifierAtIndex:(long long)arg3 groups:(id)arg4;
- (BOOL)getGroup:(long long*)arg1 row:(long long*)arg2 ofSpecifier:(id)arg3;
- (void)reloadSpecifierID:(id)arg1 animated:(BOOL)arg2;
- (id)specifierForID:(id)arg1;
- (void)reloadSpecifier:(id)arg1 animated:(BOOL)arg2;
- (void)reloadSpecifierAtIndex:(long long)arg1 animated:(BOOL)arg2;
- (id)specifiersInGroup:(long long)arg1;
- (long long)indexOfGroup:(long long)arg1;
- (void)createGroupIndices;
- (id)indexPathForIndex:(long long)arg1;
- (BOOL)getGroup:(long long*)arg1 row:(long long*)arg2 ofSpecifierAtIndex:(long long)arg3;
- (void)prepareSpecifiersMetadata;
- (long long)indexOfSpecifier:(id)arg1;
- (id)loadSpecifiersFromPlistName:(id)arg1 target:(id)arg2;
- (void)setSpecifierID:(id)arg1;
- (id)loadSpecifiersFromPlistName:(id)arg1 target:(id)arg2 bundle:(id)arg3;
- (void)setReusesCells:(BOOL)arg1;
- (void)setCachesCells:(BOOL)arg1;
- (void)reloadSpecifiers;
- (id)specifiers;
- (void)reloadSpecifier:(id)arg1;
- (void)setSpecifier:(id)arg1;
- (id)specifier;
- (id)bundle;
- (void)reload;
- (void)handleURL:(id)arg1;
- (void)setTitle:(id)arg1;
- (id)table;
- (id)init;
- (void)clearCache;
- (void)dealloc;
- (id)description;
- (long long)indexForIndexPath:(id)arg1;
- (BOOL)popoverControllerShouldDismissPopover:(id)arg1;
- (void)popoverController:(id)arg1 animationCompleted:(long long)arg2;
- (void)_keyboardDidHide:(id)arg1;
- (void)_setContentInset:(double)arg1;
- (void)didRotateFromInterfaceOrientation:(long long)arg1;
- (void)willAnimateRotationToInterfaceOrientation:(long long)arg1 duration:(double)arg2;
- (void)_keyboardWillHide:(id)arg1;
- (void)_keyboardWillShow:(id)arg1;
- (void)viewDidDisappear:(BOOL)arg1;
- (void)viewWillDisappear:(BOOL)arg1;
- (void)viewDidAppear:(BOOL)arg1;
- (void)dismissPopoverAnimated:(BOOL)arg1;
- (void)viewDidUnload;
- (void)viewDidLoad;
- (void)viewWillAppear:(BOOL)arg1;
- (void)loadView;
- (void)endUpdates;
- (void)beginUpdates;
- (long long)tableView:(id)arg1 titleAlignmentForFooterInSection:(NSInteger)arg2;
- (long long)tableView:(id)arg1 titleAlignmentForHeaderInSection:(NSInteger)arg2;
- (id)tableView:(id)arg1 detailTextForHeaderInSection:(NSInteger)arg2;
- (void)viewDidLayoutSubviews;
- (void)traitCollectionDidChange:(id)arg1;
- (void)alertView:(id)arg1 clickedButtonAtIndex:(long long)arg2;
- (void)actionSheet:(id)arg1 didDismissWithButtonIndex:(long long)arg2;
- (void)actionSheet:(id)arg1 clickedButtonAtIndex:(long long)arg2;
- (BOOL)tableView:(id)arg1 canEditRowAtIndexPath:(id)arg2;
- (id)tableView:(id)arg1 titleForFooterInSection:(NSInteger)arg2;
- (id)tableView:(id)arg1 titleForHeaderInSection:(NSInteger)arg2;
- (long long)numberOfSectionsInTableView:(id)arg1;
- (id)tableView:(id)arg1 cellForRowAtIndexPath:(id)arg2;
- (long long)tableView:(id)arg1 numberOfRowsInSection:(NSInteger)arg2;
- (void)tableView:(id)arg1 didSelectRowAtIndexPath:(id)arg2;
- (id)tableView:(id)arg1 viewForFooterInSection:(NSInteger)arg2;
- (id)tableView:(id)arg1 viewForHeaderInSection:(NSInteger)arg2;
- (double)tableView:(id)arg1 estimatedHeightForRowAtIndexPath:(id)arg2;
- (double)tableView:(id)arg1 heightForFooterInSection:(NSInteger)arg2;
- (double)tableView:(id)arg1 heightForHeaderInSection:(NSInteger)arg2;
- (double)tableView:(id)arg1 heightForRowAtIndexPath:(id)arg2;
- (void)tableView:(id)arg1 didEndDisplayingCell:(id)arg2 forRowAtIndexPath:(id)arg3;

@end

@interface PSSpecifier : NSObject {
@public
    id target;
    SEL getter;
    SEL setter;
    SEL action;
    Class detailControllerClass;
    PSCellType cellType;
    Class editPaneClass;
    UIKeyboardType keyboardType;
    UITextAutocapitalizationType autoCapsType;
    UITextAutocorrectionType autoCorrectionType;
    int textFieldType;
@private
    NSString* _name;
    NSArray* _values;
    NSDictionary* _titleDict;
    NSDictionary* _shortTitleDict;
    id _userInfo;
    NSMutableDictionary* _properties;
}
@property(retain) NSMutableDictionary* properties;
@property(retain) NSString* identifier;
@property(retain) NSString* name;
@property(retain) id userInfo;
@property(retain) id titleDictionary;
@property(retain) id shortTitleDictionary;
@property(retain) NSArray* values;
+(id)preferenceSpecifierNamed:(NSString*)title target:(id)target set:(SEL)set get:(SEL)get detail:(Class)detail cell:(PSCellType)cell edit:(Class)edit;
+(PSSpecifier*)groupSpecifierWithName:(NSString*)title;
+(PSSpecifier*)emptyGroupSpecifier;
+(UITextAutocapitalizationType)autoCapsTypeForString:(PSSpecifier*)string;
+(UITextAutocorrectionType)keyboardTypeForString:(PSSpecifier*)string;
-(id)propertyForKey:(NSString*)key;
-(void)setProperty:(id)property forKey:(NSString*)key;
-(void)removePropertyForKey:(NSString*)key;
-(void)loadValuesAndTitlesFromDataSource;
-(void)setValues:(NSArray*)values titles:(NSArray*)titles;
-(void)setValues:(NSArray*)values titles:(NSArray*)titles shortTitles:(NSArray*)shortTitles;
-(void)setupIconImageWithPath:(NSString*)path;
-(NSString*)identifier;
-(void)setTarget:(id)target;
-(void)setKeyboardType:(UIKeyboardType)type autoCaps:(UITextAutocapitalizationType)autoCaps autoCorrection:(UITextAutocorrectionType)autoCorrection;
@end

@interface PSTableCell : UITableViewCell
{
    id _value;
    UIImageView *_checkedImageView;
    _Bool _checked;
    _Bool _shouldHideTitle;
    NSString *_hiddenTitle;
    int _alignment;
    SEL _pAction;
    id _pTarget;
    _Bool _cellEnabled;
    PSSpecifier *_specifier;
    long long _type;
    _Bool _lazyIcon;
    _Bool _lazyIconDontUnload;
    _Bool _lazyIconForceSynchronous;
    NSString *_lazyIconAppID;
    _Bool _reusedCell;
    _Bool _isCopyable;
    UILongPressGestureRecognizer *_longTapRecognizer;
}

+ (Class)cellClassForSpecifier:(id)arg1;
+ (long long)cellStyle;
+ (id)reuseIdentifierForSpecifier:(id)arg1;
+ (id)reuseIdentifierForClassAndType:(long long)arg1;
+ (id)reuseIdentifierForBasicCellTypes:(long long)arg1;
+ (id)stringFromCellType:(long long)arg1;
+ (long long)cellTypeFromString:(id)arg1;
@property(retain, nonatomic) UILongPressGestureRecognizer *longTapRecognizer; // @synthesize longTapRecognizer=_longTapRecognizer;
@property(retain, nonatomic) PSSpecifier *specifier; // @synthesize specifier=_specifier;
- (double)textFieldOffset;
- (void)reloadWithSpecifier:(id)arg1 animated:(_Bool)arg2;
- (_Bool)cellEnabled;
- (void)setCellEnabled:(_Bool)arg1;
- (SEL)cellAction;
- (void)setCellAction:(SEL)arg1;
- (id)cellTarget;
- (void)setCellTarget:(id)arg1;
- (SEL)action;
- (void)setAction:(SEL)arg1;
- (id)target;
- (void)setTarget:(id)arg1;
- (void)setAlignment:(int)arg1;
- (id)iconImageView;
- (id)valueLabel;
- (id)titleLabel;
- (id)value;
- (void)setValue:(id)arg1;
- (void)setIcon:(id)arg1;
- (_Bool)canBeChecked;
- (_Bool)isChecked;
- (void)setChecked:(_Bool)arg1;
- (void)setShouldHideTitle:(_Bool)arg1;
- (void)setTitle:(id)arg1;
- (id)title;
- (id)getIcon;
- (void)forceSynchronousIconLoadOnNextIconLoad;
- (void)cellRemovedFromView;
- (id)blankIcon;
- (id)getLazyIconID;
- (id)getLazyIcon;
- (id)_contentString;
- (_Bool)canReload;
- (void)setHighlighted:(_Bool)arg1 animated:(_Bool)arg2;
- (void)setSelected:(_Bool)arg1 animated:(_Bool)arg2;
- (id)titleTextLabel;
- (void)setValueChangedTarget:(id)arg1 action:(SEL)arg2 specifier:(id)arg3;
- (void)layoutSubviews;
- (void)prepareForReuse;
- (void)refreshCellContentsWithSpecifier:(id)arg1;
- (_Bool)canPerformAction:(SEL)arg1 withSender:(id)arg2;
- (void)copy:(id)arg1;
- (id)_copyableText;
- (void)longPressed:(id)arg1;
- (_Bool)canBecomeFirstResponder;
- (void)dealloc;
- (id)initWithStyle:(long long)arg1 reuseIdentifier:(id)arg2 specifier:(id)arg3;

@end

@interface PSTextView

- (void)setCell:(id)arg1;

@end

@interface PSTextViewTableCell : PSTableCell
{
    PSTextView *_textView;
}

- (void)drawTitleInRect:(struct CGRect)arg1 selected:(_Bool)arg2;
@property(retain, nonatomic) PSTextView *textView;
- (_Bool)resignFirstResponder;
- (_Bool)canBecomeFirstResponder;
- (_Bool)becomeFirstResponder;
- (void)textContentViewDidEndEditing:(id)arg1;
- (void)_adjustTextView:(id)arg1 updateTable:(_Bool)arg2 withSpecifier:(id)arg3;
- (void)layoutSubviews;
- (void)cellRemovedFromView;
- (void)textContentViewDidChange:(id)arg1;
- (void)setValue:(id)arg1;
- (void)dealloc;
- (id)initWithStyle:(long long)arg1 reuseIdentifier:(id)arg2 specifier:(id)arg3;

@end

@interface PSTextFieldSpecifier : PSSpecifier
{
    SEL bestGuess;
    NSString *_placeholder;
    NSString *_suffix;
}

+ (id)preferenceSpecifierNamed:(id)fp8 target:(id)fp12 set:(SEL)fp16 get:(SEL)fp20 detail:(Class)fp24 cell:(int)fp28 edit:(Class)fp32;
- (void)dealloc;
- (void)setPlaceholder:(id)fp8;
- (id)placeholder;
- (void)setSuffix:(id)fp8;
- (id)suffix;

@end

@interface PSEditableTableCell : PSTableCell <UITextViewDelegate, UITextFieldDelegate>
{
    UIColor *_textColor;
    id _delegate;
    _Bool _forceFirstResponder;
    _Bool _delaySpecifierRelease;
    SEL _targetSetter;
    id _realTarget;
    _Bool _valueChanged;
    _Bool _returnKeyTapped;
    PSListController *_controllerDelegate;
}

+ (long long)cellStyle;
@property(readonly, nonatomic) _Bool returnKeyTapped; // @synthesize returnKeyTapped=_returnKeyTapped;
- (id)textField;
- (void)setPlaceholderText:(id)arg1;
- (void)setValue:(id)arg1;
- (id)value;
- (_Bool)_cellIsEditing;
- (_Bool)isEditing;
- (_Bool)isTextFieldEditing;
- (_Bool)resignFirstResponder;
- (_Bool)becomeFirstResponder;
- (_Bool)isFirstResponder;
- (_Bool)canResignFirstResponder;
- (_Bool)canBecomeFirstResponder;
- (void)layoutSubviews;
- (void)setDelegate:(id)arg1;
- (void)setTitle:(id)arg1;
- (void)setCellEnabled:(_Bool)arg1;
- (void)setValueChangedTarget:(id)arg1 action:(SEL)arg2 specifier:(id)arg3;
- (_Bool)textFieldShouldReturn:(id)arg1;
- (void)textFieldDidEndEditing:(id)arg1;
- (_Bool)textFieldShouldClear:(id)arg1;
- (void)textFieldDidBeginEditing:(id)arg1;
- (void)_saveForExit;
- (void)_setValueChanged;
- (void)cellRemovedFromView;
- (void)endEditingAndSave;
- (void)controlChanged:(id)arg1;
- (_Bool)canReload;
- (void)prepareForReuse;
- (void)refreshCellContentsWithSpecifier:(id)arg1;
- (void)dealloc;
- (id)initWithStyle:(long long)arg1 reuseIdentifier:(id)arg2 specifier:(id)arg3;
@end

@interface UITableViewCellLayoutManagerEditable1
{
}

- (void)_textValueChanged:(id)arg1;
- (void)_textFieldEndEditingOnReturn:(id)arg1;
- (void)_textFieldEndEditing:(id)arg1;
- (void)_textFieldStartEditing:(id)arg1;
- (_Bool)textFieldShouldReturn:(id)arg1;
- (_Bool)textFieldShouldBeginEditing:(id)arg1;
- (void)textFieldDidEndEditing:(id)arg1;
- (void)textFieldDidBeginEditing:(id)arg1;
- (id)editableTextFieldForCell:(id)arg1;
- (id)detailTextLabelForCell:(id)arg1;
- (void)layoutSubviewsOfCell:(id)arg1;
- (double)defaultTextFieldFontSizeForCell:(id)arg1;

@end


@interface PSListItemsController : PSListController {
    int _rowToSelect;
    BOOL _deferItemSelection;
    PSSpecifier* _lastSelectedSpecifier;
}
// inherited: -(void)viewWillRedisplay;
// inherited: -(void)viewWillBecomeVisible:(void*)view;
-(void)scrollToSelectedCell;
-(void)setRowToSelect;
-(void)setValueForSpecifier:(id)specifier defaultValue:(id)value;
// inherited: -(void)dealloc;
// inherited: -(void)suspend;
// inherited: -(void)setSpecifiers:(id)specifiers;
-(BOOL)preferencesTable:(id)table isRow:(int)row checkedInRadioGroup:(int)radioGroup;
-(BOOL)preferencesTable:(id)table isRadioGroup:(int)group;
// inherited: -(id)table:(id)table cellForRow:(int)row column:(id)column;
-(void)listItemSelected:(id)selected;
// inherited: -(void)tableSelectionDidChange:(id)tableSelection;
-(void)_addStaticText:(id)text;
-(id)itemsFromParent;
-(id)itemsFromDataSource;
// inherited: -(id)specifiers;
@end


@interface PSLanguage : NSObject  {
    NSString *_languageCode;
    NSString *_languageName;
    NSString *_localizedLanguageName;
}

@property(retain) NSString * languageCode;
@property(retain) NSString * languageName;
@property(retain) NSString * localizedLanguageName;

+ (id)languageWithCode:(id)arg1 name:(id)arg2 localizedName:(id)arg3;

- (bool)displayNamesAreEqual;
- (id)languageName;
- (void)setLocalizedLanguageName:(NSString*)arg1;
- (void)setLanguageName:(NSString*)arg1;
- (void)setLanguageCode:(NSString*)arg1;
- (id)languageCode;
- (void)dealloc;
- (id)localizedLanguageName;

@end

@interface PSLanguageSelector : NSObject  {
    NSString *_language;
    NSArray *_languagesWithLocaleData;
    NSArray *_otherLanguages;
}

+ (id)languageArrayAfterSettingLanguage:(id)arg1 fallback:(id)arg2 toLanguageArray:(id)arg3;
+ (id)sharedInstance;

- (id)languagesWithAvailableLocaleIdentifiers;
- (void)setLanguage:(id)arg1 fallback:(id)arg2;
- (id)renderableLanguagesFromList:(id)arg1;
- (bool)languageIsSupportedLanguage:(id)arg1;
- (id)defaultOtherLanguages;
- (id)userDeviceLanguageOrder;
- (id)appleLanguages;
- (id)deviceLanguageIdentifier;
- (id)supportedLanguages;
- (void)setLanguage:(id)arg1;
- (id)systemLanguages;
- (void)dealloc;
- (bool)preferredLanguages;

@end

@interface PSLanguageTableViewCell : PSTableCell  {
}

+ (long long)cellStyle;


@end

