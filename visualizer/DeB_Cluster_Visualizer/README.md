# DeB Cluster Visualizer

Collection of applications that deal with hierarchical clusters of points. This collection contains
the following applications:

- [Graph printer](#graph-printer): Application that prints the 2D graph of hierarchical clusters of
  2-dimensional points;
- [Dendrogram printer](#graph-printer): Application that prints the dendrogram tree representing the
  hierarchy of clusters;
- [Distance computer](#graph-printer): Application that computes the distance between two points.

## Graph printer

A simple application that prints on a cartesian plane the 2D representation of a hierarchy of
clusters of points.
In particular, each point is represented on the cartesian plane by a tiny circle, while each cluster
is represented as an ellipse that encloses all the points belonging to it.
The resulting graph is then stored in an image.

The information about the points and the clusters are read from an external file, which must follow
the correct format (see [File format](#file-format) for further details).

Since the graph is 2-dimensional, only 2-dimensional points are allowed. If hyper-dimensional points
are specified, then this application immediately terminates the execution.

### Usage

The application can be invoked in two different ways, according to the command line arguments
specified:

```
graph-printer -h
```

or

```
graph-printer INPUT_FILE_PATH OUTPUT_FILE_PATH IMAGE_WIDTH IMAGE_HEIGHT
```

The first way just prints the usage of the application and terminates the execution.

The second, instead, reads and parses the content of the `INPUT_FILE_PATH` file, generates an image
with size `IMAGE_WIDTH &times; IMAGE_HEIGHT` of the 2D graph, and then stores it in the
file `OUTPUT_FILE_PATH`.

If some errors occur during the generation of the image (e.g., the input file does not follow the
format specified in the [File format](#file-format) section), then the output file is left
untouched.

Both the `INPUT_FILE_PATH` and `OUTPUT_FILE_PATH` paths can be either absolute, or relative to the
current working directory. Symbolic links in the paths are followed.

Moreover, the `INPUT_FILE_PATH` must refer to an existent regular file, while `OUTPUT_FILE_PATH`
can refer to a non-existent file, provided that the directory that will contain it exists. In this
case, the output file is created when the image is stored.

Note that both `IMAGE_WIDTH` and `IMAGE_HEIGHT` must be positive numbers.

### Run the application with Gradle

The main Gradle script provides a specific `JavaExec` _task_, named `runGraphPrinter`, that allows
to run this application. To invoke it, just execute:

```
gradle runGraphPrinter --args="<arguments>"
```

## Dendrogram printer

A simple application that generates the dendrogram tree representing the hierarchical relations
between clusters of points. Like the _graph printer_ application, the information about the points
and the clusters are read from an external file, which must follow the format explained in
the [File format](#file-format) section.

### Usage

The application can be invoked in two different ways, according to the command line arguments
specified:

```
dendrogram-printer -h
```

or

```
dendrogram-printer INPUT_FILE_PATH OUTPUT_FILE_PATH IMAGE_WIDTH IMAGE_HEIGHT
```

The first way just prints the usage of the application and terminates the execution.

The second, instead, reads and parses the content of the `INPUT_FILE_PATH` file, generates an image
with size `IMAGE_WIDTH &times; IMAGE_HEIGHT` of the dendrogram tree, and then stores it in the
file `OUTPUT_FILE_PATH`.

If some errors occur during the generation of the image (e.g., the input file does not follow the
format specified in the [File format](#file-format) section), then the output file is left
untouched.

Both the `INPUT_FILE_PATH` and `OUTPUT_FILE_PATH` paths can be either absolute, or relative to the
current working directory. Symbolic links in the paths are followed.

Moreover, the `INPUT_FILE_PATH` must refer to an existent regular file, while `OUTPUT_FILE_PATH`
can refer to a non-existent file, provided that the directory that will contain it exists. In this
case, the output file is created when the image is stored.

Note that both `IMAGE_WIDTH` and `IMAGE_HEIGHT` must be positive numbers.

### Run the application with Gradle

The main Gradle script provides a specific `JavaExec` _task_, named `runDendrogramPrinter`, that
allows to run this application. To invoke it, just execute:

```
gradle runDendrogramPrinter --args="<arguments>"
```

## Distance computer

This simple application computes the Euclidean distance between two points.

### Usage

Unlike the previous applications, this one is interactive.

In particular, when run, the application first asks the dimension of the points (i.e., the number of
coordinates of each point), which must be a positive number.
Then, the application asks the user to provide all the coordinates of the two points, one at a time.
Finally, when all the coordinates have been given, the application computes and prints the Euclidean
distance between the specified points.

### Run the application with Gradle

Like the previous applications, the main Gradle script provides a specific `JavaExec` _task_,
named `runDistanceComputer`, that allows to run this application. To invoke it, just execute:

```
gradle runDistanceComputer
```

## File Format

The input file for both the _graph printer_ and _dendrogram printer_ applications is composed of two
sections, mainly the `<points>` section and the `<clusters>` section. They are specified exactly in
this order, and they are put one after the other without any particular separator, i.e., they
follow the following format (note the new lines):

```
<points>
<clusters>
```

### `<points>` section

This section specifies the sequence of points. Each point is put on its own line, and it must comply
with the following format (note the spaces):

```
P<number>: "<name>" <coordinates ...>
```

where:

- `<number>` is a sequence of digits that uniquely identifies the point;
- `<name>` is the name of the point. This sequence of characters is used as text of label
  printed to identify the point in the graph or in the dendrogram;
- `<coordinates ...>` is the sequence of coordinates of the point in the Euclidean space. Each
  coordinate must be a valid real number, and must be separated from the following one by a
  space. No space is needed after the last coordinate.

  This field is optional, hence it can be omitted. If so, the points are assumed to be non
  2-dimensional, which implies that cannot be used in applications that require the points to be
  2-dimensional.

### `<clusters>` section

This section specifies the sequence of clusters. Each cluster is put on its own line and must follow
the following format (note the spaces):

```
C<number>: "<name>" <first sub-cluster id> <second sub-cluster id> <distance>
```

where:

- `<number>` is a sequence of digits that uniquely identifies the cluster;
- `<name>` is the name of the cluster. This sequence of characters is used in all the contexts
  where a label associated to the cluster is printed to the image;
- `<first sub-cluster id>` identifier of the first sub-cluster of the cluster according to the
  hierarchy of clusters:
    
    - If the sub-cluster contains only one point, then this field must be specified in the
      form `P<point number>`, where `<point number>` is the identifier of the only point belonging
      to the sub-cluster. Note that `<point number>` must refer to a point specified in
      the `<points>` section;
    
    - If, instead, the sub-cluster is in turn a cluster, then this field must be specified in the
      form `C<cluster number>`, where `<cluster number>` is the identifier of the sub-cluster. This
      identifier must refer to a cluster in the `<clusters>` section, although the cluster with that
      identifier may be defined after this cluster. Therefore, it is not mandatory that a cluster is
      defined before it is referred;
- `<second sub-cluster id>` identifier of the second sub-cluster of the cluster according to the
  hierarchy of clusters.

  This field follows the same format of the `<first sub-cluster id>` one, so refer to it for further
  details;
- `<distance>` Distance between the sub-cluster identified by `<first sub-cluster id>` and the one
  identified by `<second sub-cluster id>`. This field must be a valid non-negative real number.

### Example

The following is an example of a file that follows the format:

```
P0: "1" 1.5 2.7
P1: "2" 1 -3
P2: "3" 4.9 5
P3: "4" -6.1 9.06
P4: "5" 1.007 -3.8
P5: "6" -7.00064 -1.005
C4: "Root cluster" P3 C3 9.91008
C3: "Cluster 13256" C2 P5 8.24562
C0: "Cluster 25" P1 P4 0.800031
C1: "Cluster 13" P0 P2 4.10488
C2: "Cluster 1325" C1 C0 5.72189
```
