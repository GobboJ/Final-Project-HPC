package it.debsite.dcv.main;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.presenter.CanvasPresenter;
import it.debsite.dcv.presenter.GraphPresenter;
import it.debsite.dcv.view.GraphPrinter;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.WritableImage;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.BorderPane;
import javafx.stage.Stage;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-09
 * @since version date
 */
public class MainApplication extends Application {
    
    public static void main(final String[] args) {
        
        Application.launch(args);
    }
    
    @Override
    public void start(final Stage primaryStage) {
    
        
        ClusterPoint a = new ClusterPoint("A", 1, 1);
        ClusterPoint b = new ClusterPoint("B", 1.5, 1.5);
        ClusterPoint c = new ClusterPoint("C", 5, 5);
        ClusterPoint d = new ClusterPoint("D", 3, 4);
        ClusterPoint e = new ClusterPoint("E", 4, 4);
        ClusterPoint f = new ClusterPoint("F", 3, 3.5);
        
        List<ClusterPoint> points = new ArrayList<>();
        Collections.addAll(points, a, b, c, d, e, f);
        
        List<ClusterPoint> abCluster = new ArrayList<>();
        Collections.addAll(abCluster, a, b);
        
        List<ClusterPoint> dfCluster = new ArrayList<>();
        Collections.addAll(dfCluster, d, f);
        
        List<ClusterPoint> dfeCluster = new ArrayList<>();
        Collections.addAll(dfeCluster, d, f, e);
        
        List<ClusterPoint> dfecCluster = new ArrayList<>();
        Collections.addAll(dfecCluster, d, f, e, c);
        
        List<ClusterPoint> allCluster = new ArrayList<>();
        Collections.addAll(allCluster, a, b, d, f, e, c);
        
        List<Cluster> clusters = new ArrayList<>();
        Collections.addAll(
            clusters,
            new Cluster("AB", abCluster),
            new Cluster("DF", dfCluster),
            new Cluster("DFE", dfeCluster),
            new Cluster("DFEC", dfecCluster),
            new Cluster("ALL", allCluster)
        );
        
        try {
            final FXMLLoader fxmlLoader =
                new FXMLLoader(MainApplication.class.getResource("/fxml/window.fxml"));
            final BorderPane root = fxmlLoader.load();
            Scene scene = new Scene(root);
            primaryStage.setScene(scene);
            primaryStage.show();
            
            AnchorPane canvasContainer = (AnchorPane) scene.lookup("#graph-container");
            Canvas canvas = (Canvas) scene.lookup("#graph-canvas");
            GraphPrinter graphPrinter = new GraphPrinter(canvas);
            
            CanvasPresenter presenter = new CanvasPresenter(canvasContainer, canvas);
            GraphPresenter graphPresenter =
                new GraphPresenter(points, clusters, canvas, graphPrinter);
            
        } catch (IOException exception) {
            // TODO: Auto-generated block
            exception.printStackTrace();
        }
    }
}
