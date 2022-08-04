/**
 * Module containing all the presenter-related classes.
 *
 * @author DeB
 * @since 1.0
 */
module it.debsite.dcv.presenter {
    
    // Model classes
    requires it.debsite.dcv.model;
    // View classes
    requires it.debsite.dcv.view;
    
    // AWT
    requires java.desktop;
    
    // Annotations
    requires static org.jetbrains.annotations;
    requires static lombok;
    
    // Exported packages
    exports it.debsite.dcv.presenter;
}
