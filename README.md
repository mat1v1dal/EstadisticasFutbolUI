# ⚽ Guía de Uso de la Aplicación

## Introducción
Esta aplicación permite gestionar y visualizar estadísticas de fútbol mediante una interfaz gráfica desarrollada con **Qt**. A continuación, se detallan los pasos para ejecutar la aplicación y comenzar a utilizarla.

## Instrucciones de Uso

### 1. 📁 Ubicación del Archivo Ejecutable
Dentro de la **raíz del proyecto**, encontrará una carpeta llamada `Ejecucion`. Esta carpeta contiene todos los archivos necesarios para ejecutar la aplicación, incluyendo el archivo ejecutable principal.

- **Ruta**: `<raiz_del_proyecto>/Ejecucion/EstadisticasFutbol.exe`

### 2. ▶️ Ejecución de la Aplicación
1. **Navegue a la carpeta `Ejecucion`**.
2. **Ejecute el archivo `EstadisticasFutbol.exe`** haciendo doble clic sobre él.

### 3. 📂 Carga de Datos
Al iniciar la aplicación, se le solicitará al usuario que especifique la **ruta del archivo de datos** que contiene la información de las estadísticas de fútbol.

- Para la presentación del examen, este archivo de datos se encuentra en la carpeta `data` en la **raíz del proyecto**:
  - **Ruta**: `<raiz_del_proyecto>/data`

### 4. 🖥️ Uso de la Aplicación
Una vez seleccionada la ruta del archivo de datos, la aplicación cargará automáticamente la información y estará lista para su uso. A partir de este momento, podrá interactuar con las distintas funciones de la aplicación sin inconvenientes.

## Requisitos Adicionales

### ✅ Aplicación Lista para su Uso
La aplicación proporcionada en la carpeta `Ejecucion` ya está **compilada y lista para su uso**, por lo que no necesita realizar ningún paso adicional para ejecutar el programa.

### 🛠️ Realizar Cambios en el Proyecto
Si desea modificar o compilar nuevamente la aplicación, deberá contar con la herramienta **windeployqt**. Esta herramienta es parte del entorno de desarrollo de **Qt** y se utiliza para compilar y desplegar todas las bibliotecas necesarias de **Qt**.

- **windeployqt** asegura que todas las dependencias de **Qt** estén incluidas junto con el ejecutable, lo que permite un despliegue correcto en sistemas que no tengan **Qt** preinstalado.

### 🔧 Compilación y Configuración
1. **Realice los cambios en el código** que desee.
2. **Compile el proyecto** utilizando **Qt Creator** u otra herramienta compatible.
3. **Utilice `windeployqt`** para incluir todas las bibliotecas necesarias:
   ```bash
   windeployqt <ruta_al_exe>
# Proyecto Estadísticas de Fútbol - Documentación 📝

Este proyecto está diseñado siguiendo el principio de **Clean Architecture**, lo que permite una clara separación de responsabilidades y asegura un código mantenible, escalable y fácil de testear. La estructura del proyecto se divide en cuatro capas principales:

1. **Capa de Entidades (Core)**
2. **Capa de Servicios**
3. **Capa de Control de Base de Datos (DB Control)**
4. **Capa de Vista**

A continuación, se detallan las responsabilidades y características de cada capa, así como la justificación de su inclusión en el diseño general del proyecto.

## 1. Capa de Entidades (Core) 🧩
### Descripción
La **capa de entidades** es la base del proyecto. Contiene las clases principales que representan las entidades del dominio, encapsulando la lógica de negocio y las reglas de la aplicación. Esta capa es completamente independiente de cualquier implementación tecnológica y se centra en definir los conceptos fundamentales de la aplicación.

### Ejemplo de Contenido
- Clases de dominio como `Equipo`, `Partido`, `Estadisticas`, etc.
- Estructuras de datos y métodos necesarios para manipular estas clases.

### Justificación
La separación de la capa de entidades garantiza que la lógica de negocio esté desacoplada de los detalles de implementación, facilitando el testeo y la modificación futura del código sin afectar otras capas. Además, permite reutilizar las entidades en distintos contextos o aplicaciones.

## 2. Capa de Servicios 🔄
### Descripción
La **capa de servicios** utiliza las entidades de la capa de Core para construir y operar sobre estructuras de datos. Actúa como un puente entre la lógica de negocio y las operaciones que deben realizarse en la aplicación.

### Ejemplo de Contenido
- Lógica para el cálculo de estadísticas agregadas.
- Métodos que manejan transformaciones de datos complejos.
- Servicios de validación de datos y análisis de partidos.
# Documentación de `ServicioPartidoTree`

## Estructuras de Datos Utilizadas

### 1. `std::unordered_map`
- **Uso**: 
  - Se utiliza para almacenar las competiciones en el mapa `competiciones`.
  - Se utiliza un `unordered_map` anidado para almacenar equipos por competición en el mapa `equipos`.
  
- **Justificación**: 
  - Esta estructura de datos es un hash map que permite acceso, inserción y eliminación en promedio O(1).
  - Es ideal para situaciones donde se requiere un acceso rápido por clave, como buscar una competición por nombre o verificar la existencia de un equipo en una competición específica.

- **Ventajas**:
  - **Velocidad de acceso**: Las operaciones comunes (búsqueda, inserción, eliminación) se realizan en tiempo constante en promedio, lo que optimiza el rendimiento al manejar un gran número de competiciones y equipos.

### 2. `ArbolBinarioAVL<Partido>`
- **Uso**: 
  - Se utiliza un árbol AVL para almacenar los partidos de cada competición.
  
- **Justificación**: 
  - El árbol AVL es un tipo de árbol binario de búsqueda auto-balanceado que garantiza que las operaciones de inserción, eliminación y búsqueda se realicen en O(log n) en el peor de los casos.
  - Esto es crucial para mantener un rendimiento eficiente al manipular listas de partidos, especialmente cuando se requieren operaciones frecuentes de búsqueda y modificación.

- **Ventajas**:
  - **Balanceo automático**: Asegura que el árbol permanezca equilibrado, proporcionando tiempos de operación óptimos incluso a medida que el número de partidos crece.
  - **Ordenado**: Permite realizar recorridos en orden para acceder a los partidos de manera secuencial, facilitando operaciones que dependen de la ordenación de los partidos.

### 3. `std::vector<Partido>`
- **Uso**: 
  - Se utiliza para almacenar listas de partidos, como en los métodos que obtienen todos los partidos en una competición o filtran partidos por diferentes criterios.

- **Justificación**: 
  - `std::vector` es una estructura de datos dinámica que permite almacenar elementos de forma contigua en memoria.
  - Proporciona acceso aleatorio en O(1) y permite redimensionarse según sea necesario, lo que es útil para manejar colecciones de partidos de longitud variable.

- **Ventajas**:
  - **Eficiencia en acceso y recorrido**: Permite un acceso rápido a los elementos y es eficiente para operaciones de recorrido, como buscar partidos específicos.
  - **Flexibilidad**: Se adapta fácilmente al tamaño variable de los conjuntos de datos, lo que es ideal en un contexto donde el número de partidos puede cambiar con frecuencia.

## Conclusiones
Las estructuras de datos elegidas para `ServicioPartidoTree` están diseñadas para maximizar la eficiencia en el acceso y la manipulación de datos relacionados con competiciones y partidos. La combinación de `std::unordered_map`, `ArbolBinarioAVL`, y `std::vector` proporciona un balance adecuado entre rapidez de acceso, eficiencia de operaciones, y flexibilidad en el manejo de datos.
### Justificación
Esta capa ayuda a separar la lógica de negocio de alto nivel y a orquestar procesos complejos. Las funcionalidades que dependen de múltiples entidades pueden mantenerse y probarse de forma independiente, lo cual mejora la robustez del proyecto.

## 3. Capa de Control de Base de Datos (DB Control) 💾
### Descripción
La **capa de control de base de datos** gestiona la interacción con la base de datos. Implementa operaciones CRUD (Create, Read, Update, Delete) y asegura que la aplicación administre el almacenamiento y recuperación de datos de manera eficiente.

### Ejemplo de Contenido
- Métodos para realizar operaciones de lectura y escritura en la base de datos.
- Gestión de conexiones y transacciones con la base de datos.
- Lógica de mapeo de datos entre entidades y tablas.

### Justificación
Centralizar la interacción con la base de datos en esta capa dedicada mejora el control sobre el acceso a los datos, facilita los cambios en el sistema de almacenamiento y mejora la seguridad y robustez de la aplicación. Además, reduce el acoplamiento directo de otras partes del código con la base de datos.

## 4. Capa de Vista 🖥️
### Descripción
La **capa de vista** conecta todas las capas anteriores y proporciona una interfaz gráfica de usuario mediante **Qt**, un framework popular para desarrollar aplicaciones de escritorio con interfaces ricas.

### Ejemplo de Contenido
- Ventanas y widgets de **Qt** que representan la UI.
- Clases que gestionan la lógica de presentación y eventos de usuario.
- Integración con la lógica de negocio a través de la capa de servicios y DB control.

### Justificación
Separar la presentación de la lógica de negocio facilita el mantenimiento y permite que cualquier cambio en la interfaz de usuario se realice sin afectar otras capas. El uso de **Qt** proporciona una experiencia de usuario fluida y profesional.

---

🔍 **Conclusión**: La división en estas capas asegura un diseño limpio y modular, donde cada parte del proyecto tiene un propósito específico y puede evolucionar independientemente.
---

