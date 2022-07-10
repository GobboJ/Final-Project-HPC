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
    
    requires javafx.base;
    requires javafx.graphics;
    requires javafx.controls;
    requires javafx.fxml;
    
   opens it.debsite.dcv.main to javafx.fxml;
    
    requires static org.jetbrains.annotations;
    
    exports it.debsite.dcv.main;
}
