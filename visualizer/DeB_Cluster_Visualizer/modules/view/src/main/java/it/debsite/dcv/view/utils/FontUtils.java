package it.debsite.dcv.view.utils;

import org.intellij.lang.annotations.MagicConstant;
import org.jetbrains.annotations.NotNull;

import javax.swing.UIManager;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.geom.Rectangle2D;

/**
 * Utility class dealing with fonts.
 *
 * @author DeB
 * @version 1.2 2022-08-02
 * @since 1.0
 */
public final class FontUtils {
    
    /**
     * Enumeration that specifies all the supported font styles when deriving the system font.
     *
     * @author DeB
     * @version 1.1 2022-08-01
     * @since 1.0
     */
    public enum FontStyle {
        
        /**
         * Bold font.
         */
        BOLD(Font.BOLD),
        
        /**
         * Plain font.
         */
        PLAIN(Font.PLAIN);
        
        /**
         * The AWT font style this object represents.
         */
        @MagicConstant(flags = {Font.BOLD, Font.PLAIN})
        private final int fontStyle;
        
        /**
         * Creates a new font style that represents the specified AWT font style.
         *
         * @param fontStyle The AWT font style this object represent.
         */
        FontStyle(@MagicConstant(flags = {Font.BOLD, Font.PLAIN}) final int fontStyle) {
            
            this.fontStyle = fontStyle;
        }
        
        /**
         * Returns the AWT font style this object represents.
         *
         * @return The AWT font style this object represents.
         */
        @MagicConstant(flags = {Font.BOLD, Font.PLAIN})
        private int getFontStyle() {
            
            return this.fontStyle;
        }
    }
    
    /**
     * The constructor is private so not to allow creating object of this class, since it offers
     * only static methods.
     */
    private FontUtils() {
    
    }
    
    /**
     * Creates a new {@link Font} object representing the system font with the specified height.
     * Note that the system font is specified by the current <i>Look and Feel</i> of the
     * application.
     *
     * @param height Height of the new font.
     * @return The requested font.
     */
    @NotNull
    public static Font deriveSystemFont(final float height) {
        
        return FontUtils.deriveSystemFont(height, FontStyle.PLAIN);
    }
    
    /**
     * Creates a new {@link Font} object representing the system font with the specified height and
     * style. Note that the system font is specified by the current <i>Look and Feel</i> of the
     * application.
     *
     * @param height Height of the new font.
     * @param fontStyle Style of the new font.
     * @return The requested font.
     * @see FontStyle
     */
    @NotNull
    public static Font deriveSystemFont(final float height, @NotNull final FontStyle fontStyle) {
        
        // Derive the font used in the Menu options
        return UIManager.getFont("Menu.font").deriveFont(fontStyle.getFontStyle(), height);
    }
    
    /**
     * Computes the bounds of the specified text.
     *
     * @param text Text to compute the bounds of.
     * @param font Font to use to render the text.
     * @param fontMetrics Font metrics of the font according to the graphic context.
     * @return The computed bounds.
     */
    @NotNull
    public static Rectangle2D getTextBounds(
        @NotNull final String text, @NotNull final Font font, @NotNull final FontMetrics fontMetrics
    ) {
        
        return font.createGlyphVector(fontMetrics.getFontRenderContext(), text).getVisualBounds();
    }
}
