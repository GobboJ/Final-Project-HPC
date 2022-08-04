/**
 * Module containing all the view-related classes.
 *
 * @author DeB
 * @since 1.0
 */
module it.debsite.dcv.view {
    
    // AWT
    requires java.desktop;
    
    // Annotations
    requires static org.jetbrains.annotations;
    requires static lombok;
    
    // Exported packages
    exports it.debsite.dcv.view;
    exports it.debsite.dcv.view.dendrogram;
    exports it.debsite.dcv.view.graph;
    exports it.debsite.dcv.view.dendrogram.tree;
    
}
