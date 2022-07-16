package it.debsite.dcv.view.draw;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.geom.Line2D;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-10
 * @since version date
 */
public class HorizontalAxisPrinter {
    
    public static void drawHorizontalLine(
        final Graphics2D context,
        final double startX,
        final double endX,
        final double y,
        final String text,
        final float lineWidth,
        final float fontHeight,
        final double textMargin,
        final boolean drawRight
    ) {
        
        context.setColor(Color.LIGHT_GRAY);
        context.setStroke(new BasicStroke(lineWidth));
        context.draw(new Line2D.Double(startX, y, endX, y));
        
        context.setColor(Color.BLACK);
        final int textWidth = context.getFontMetrics().stringWidth(text);
        context.drawString(text,
            (float) (startX - textMargin - textWidth),
            (float) (y + (fontHeight / 2))
        );
        if (drawRight) {
            context.drawString(text, (float) (endX + textMargin), (float) (y + (fontHeight / 2)));
        }
    }
}
