/**
 * Module containing all the main classes.
 *
 * @author DeB
 * @since 1.0
 */
module it.debsite.dcv.main {
    
    // Require all the other modules
    requires it.debsite.dcv.model;
    requires it.debsite.dcv.presenter;
    requires it.debsite.dcv.view;
    
    // AWT
    requires java.desktop;
    
    // Annotations
    requires static org.jetbrains.annotations;
    requires static lombok;
    
    // Export all the main classes
    exports it.debsite.dcv.main;
}
