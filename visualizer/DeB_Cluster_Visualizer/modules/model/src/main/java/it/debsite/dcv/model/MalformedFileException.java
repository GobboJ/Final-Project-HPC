package it.debsite.dcv.model;

import org.jetbrains.annotations.NotNull;

import java.io.Serial;

/**
 * Exception indicating that the file containing the information about the points and the clusters
 * is malformed.
 *
 * @author DeB
 * @version 1.1 2022-07-17
 * @since 1.0
 */
public class MalformedFileException extends Exception {
    
    /**
     * Version of this class.
     */
    @Serial
    private static final long serialVersionUID = 11L;
    
    /**
     * Creates a new exception indicating that the file containing the information about the points
     * and clusters is malformed.
     *
     * @param message Message of the exception.
     */
    public MalformedFileException(@NotNull final String message) {
        
        super(message);
    }
}
