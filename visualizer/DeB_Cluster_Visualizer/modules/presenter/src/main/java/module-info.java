/**
 * Description.
 *
 * @author DeB
 * @since version date
 */
module it.debsite.dcv.presenter {
    
    requires it.debsite.dcv.model;
    requires it.debsite.dcv.view;
    requires javafx.graphics;
    
    requires static org.jetbrains.annotations;
    requires static lombok;
    
    exports it.debsite.dcv.presenter;
}
