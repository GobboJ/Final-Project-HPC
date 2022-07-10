package it.debsite.dcv.presenter;

import javafx.scene.canvas.Canvas;
import javafx.scene.layout.AnchorPane;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class CanvasPresenter {
    
    public CanvasPresenter(AnchorPane container, Canvas canvas) {
        canvas.widthProperty().bind(container.widthProperty());
        canvas.heightProperty().bind(container.heightProperty());
    }
}
