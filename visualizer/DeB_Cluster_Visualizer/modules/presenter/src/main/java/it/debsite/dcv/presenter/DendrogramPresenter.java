package it.debsite.dcv.presenter;

import it.debsite.dcv.model.Cluster;
import it.debsite.dcv.model.ClusterPoint;
import it.debsite.dcv.presenter.utils.ScaleComputer;
import it.debsite.dcv.view.DendrogramPrinter;
import it.debsite.dcv.view.DendrogramTreeNode;
import it.debsite.dcv.view.GraphAxisLabel;

import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Objects;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-10
 * @since version date
 */
public class DendrogramPresenter {
    
    private final double MARGIN = 80;
    
    private final double BOX_MARGIN = 20;
    
    private final double BLOCK_SIZE = 60;
    
    private final BufferedImage image;
    
    private final DendrogramPrinter printer;
    
    public DendrogramPresenter(
        final BufferedImage image,
        final DendrogramPrinter printer,
        final List<Cluster> clusters,
        final List<ClusterPoint> points
    ) {
        
        this.image = image;
        this.printer = printer;
        
        final Map<String, DendrogramTreeNode> pointNodes = new HashMap<>();
        
        for (final ClusterPoint point : points) {
            final DendrogramTreeNode pointNode = new DendrogramTreeNode(point.getName(), 0, 0, 0);
            pointNodes.put(point.getIdentifier(), pointNode);
        }
        
        final Map<String, DendrogramTreeNode> clusterNodes = new HashMap<>();
        for (final Cluster cluster : clusters) {
            final DendrogramTreeNode clusterNode =
                new DendrogramTreeNode(cluster.getName(), 0, 0, cluster.getDistance());
            clusterNodes.put(cluster.getIdentifier(), clusterNode);
        }
        
        for (final Cluster cluster : clusters) {
            final DendrogramTreeNode clusterNode = clusterNodes.get(cluster.getIdentifier());
            if (clusterNode == null) {
                throw new IllegalArgumentException("Malformed cluster %s".formatted(cluster.getName()));
            }
            final DendrogramTreeNode leftNode =
                (cluster.getLeftIdentifier().startsWith("P") ? pointNodes : clusterNodes).get(
                    cluster.getLeftIdentifier());
            
            final DendrogramTreeNode rightNode =
                (cluster.getRightIdentifier().startsWith("P") ? pointNodes : clusterNodes).get(
                    cluster.getRightIdentifier());
            
            if ((leftNode == null) || (rightNode == null)) {
                throw new IllegalArgumentException("Malformed cluster %s".formatted(cluster.getName()));
            }
            
            clusterNode.setLeft(leftNode);
            clusterNode.setRight(rightNode);
            leftNode.setParent(clusterNode);
            rightNode.setParent(clusterNode);
        }
        
        // Find root
        DendrogramTreeNode root = null;
        for (final Entry<String, DendrogramTreeNode> node : clusterNodes.entrySet()) {
            if (node.getValue().getParent() == null) {
                if (root == null) {
                    root = node.getValue();
                } else {
                    throw new IllegalArgumentException("Found isolated node %s".formatted(node.getValue()
                                                                                              .getName()));
                }
            }
        }
        if (root == null) {
            throw new IllegalArgumentException("Malformed clusters");
        }
        
        final Rectangle2D.Double area = new Rectangle2D.Double(this.MARGIN,
            this.MARGIN,
            image.getWidth() - (this.MARGIN * 2),
            image.getHeight() - (this.MARGIN * 2)
        );
        
        final double xStep = area.getWidth() / (points.size() - 1);
        
        // Clusters -> only y
        // Points -> x and y
        DendrogramPresenter.setCoordinatesOf(root,
            0,
            area.getX(),
            xStep,
            area.getY(),
            area.getHeight(),
            root.getDistance()
        );
        
        final Rectangle2D.Double boxArea = new Rectangle2D.Double(this.BOX_MARGIN,
            this.BOX_MARGIN,
            image.getWidth() - (this.BOX_MARGIN * 2),
            image.getHeight() - (this.BOX_MARGIN * 2)
        );
        
        double horizontalAxisY = area.getY() + area.getHeight();
        
        // dendrogramHeight : rootDistance = 60 : ???
        double blockDistance = (root.getDistance() * BLOCK_SIZE) / area.getHeight();
        double scale = ScaleComputer.getScale(blockDistance);
        // dendrogramHeight : rootDistance = ??? : scale
        double yStep = (scale * area.getHeight()) / root.getDistance();
        List<GraphAxisLabel> yLabels = new ArrayList<>();
        
        double y = yStep;
        double value = scale;
        while ((horizontalAxisY - y) > area.getY()) {
            GraphAxisLabel label = new GraphAxisLabel("%.2f".formatted(value), horizontalAxisY - y);
            yLabels.add(label);
            y += yStep;
            value += scale;
        }
        
        this.printer.draw(root, boxArea, horizontalAxisY, area.getX() - this.BOX_MARGIN, yLabels);
    }
    
    private static int setCoordinatesOf(
        final DendrogramTreeNode node,
        final int nextLeafIndex,
        final double leftX,
        final double xStep,
        final double topY,
        final double dendrogramHeight,
        final double rootDistance
    ) {
        
        if (node.isLeaf()) {
            
            node.setX(leftX + (xStep * nextLeafIndex));
            node.setY(topY + dendrogramHeight);
            
            return nextLeafIndex + 1;
        }
        // dendrogramHeight : rootDistance = ???? : node.distance
        final double nodeHeight = (node.getDistance() * dendrogramHeight) / rootDistance;
        node.setY(topY + (dendrogramHeight - nodeHeight));
        
        final int index =
            DendrogramPresenter.setCoordinatesOf(Objects.requireNonNull(node.getLeft()),
                nextLeafIndex,
                leftX,
                xStep,
                topY,
                dendrogramHeight,
                rootDistance
            );
        return DendrogramPresenter.setCoordinatesOf(Objects.requireNonNull(node.getRight()),
            index,
            leftX,
            xStep,
            topY,
            dendrogramHeight,
            rootDistance
        );
    }
}
