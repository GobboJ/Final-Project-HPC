/**
 * Description.
 *
 * @author DeB
 * @since version date
 */
module it.debsite.dcv.main {
    
    requires it.debsite.dcv.model;
    requires it.debsite.dcv.presenter;
    requires it.debsite.dcv.view;
    requires java.desktop;
    
    requires static org.jetbrains.annotations;
    
    exports it.debsite.dcv.main;
}
