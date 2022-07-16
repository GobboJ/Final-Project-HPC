package it.debsite.dcv.view.draw;

import javax.swing.UIManager;
import java.awt.Font;

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-16
 * @since version date
 */
public class SystemFont {
    
    public static Font deriveSystemFont(final float height) {
        
        return SystemFont.deriveSystemFont(height, Font.PLAIN);
    }
    
    public static Font deriveSystemFont(final float height, final int fontStyle) {
        
        return UIManager.getFont("Menu.font").deriveFont(fontStyle, height);
    }
}
