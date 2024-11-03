#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./servicios/ServicioPartido.h"
#include <vector>
#include "./dbcontroll/DataControl.h"
#include <QString>
#include <chrono>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<Partido>("Partido");
    obtenerRutaArchivo();
    obtenerPartidos(servicioPartido, rutaArchivo);
    cargarPartidosEnListWidget();

    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::onItemSelectionChanged);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::on_editarButton_clicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::on_eliminarButton_clicked);
    connect(ui->agregarPartidoButton, &QPushButton::clicked, this, &MainWindow::on_agregarButton_clicked);
    connect(ui->top5Button, &QPushButton::clicked, this, [this]() {
        mostrarTop5Partidos(servicioPartido);
    });
    connect(ui->reiniciarWidget, &QPushButton::clicked, this, [this]() {
        resetWidget();
    });
    connect(ui->golesTotalesButton, &QPushButton::clicked, this, [this]() {
        mostrarGolesTotalesPorEquipo(servicioPartido);
    });
    connect(ui->promedioGolesButton, &QPushButton::clicked, this, [this]() {
        mostrarPromedioGolesPorEquipo(servicioPartido);
    });

    connect(ui->victoriasDerrotasButton, &QPushButton::clicked, this, [this]() {
        mostrarVictoriasYDerrotasPorCompeticion(servicioPartido);
    });
    connect(ui->masYmenosGolesButton, &QPushButton::clicked, this, [this]() {
        competicionConMasGoles(servicioPartido);
    });
    connect(ui->fechaGolesButton, &QPushButton::clicked, this, [this]() {
        mostrarFechaConMasYMenosGoles(servicioPartido);
    });
    connect(ui->filtrarUmbralButton, &QPushButton::clicked, this, [this]() {
        filtrarPartidosPorUmbral(servicioPartido);
    });
    connect(ui->equipoMasGolesButton, &QPushButton::clicked, this, [this]() {
        mostrarEquiposConMasYMenosGoles(servicioPartido);
    });
    connect(ui->partidosEntreFechasButton, &QPushButton::clicked, this, [this]() {
        buscarPartidosEntreFechasButton(servicioPartido);
    });


    connect(ui->compararDosEquiposButton, &QPushButton::clicked, this, [this]() {
        compararRendimientoEquipos(servicioPartido);
    });
    ui->editButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);

}

void MainWindow::actualizarVectorDePartidos(const std::string& rutaArchivo){
    DataControl dataControl;
    partidos = dataControl.obtenerPartidos(rutaArchivo);
}

void MainWindow::obtenerRutaArchivo(){
    QString currentPath = QDir::currentPath();
    QString directory = QFileDialog::getOpenFileName(
        nullptr,
        "Seleccionar Archivo de Datos",
        currentPath,  // Usar el directorio actual como la ruta por defecto
        "Archivos CSV (*.csv);;Todos los archivos (*)"
        );

    rutaArchivo = directory.toStdString();

    if (directory.isEmpty()) {
        QMessageBox::warning(nullptr, "Sin selección", "No se seleccionó ninguna carpeta. La aplicación se cerrará.");
    }
}

void MainWindow::obtenerPartidos(ServicioPartidoTree &servicio, const std::string& rutaArchivo) {
    DataControl dataControl;
    int contadorIfs = 0;
    auto start = std::chrono::high_resolution_clock::now();
    partidos = dataControl.obtenerPartidos(rutaArchivo);
    contadorIfs += servicio.getContadorIfs();
    for(auto& partido : partidos) {
        servicio.registrarEquipo(partido.getLiga(), partido.getEquipoLocalObj());
        contadorIfs += servicio.getContadorIfs();
        servicio.registrarEquipo(partido.getLiga(), partido.getEquipoVisitanteObj());
        contadorIfs += servicio.getContadorIfs();
        servicio.actualizarEstadisticasPorCompeticion(partido);
        contadorIfs += servicio.getContadorIfs();
        servicio.registrarPartidoEnHash(partido);
        contadorIfs += servicio.getContadorIfs();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Mostrar mensaje de éxito con el tiempo de carga
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(contadorIfs));
}

void MainWindow::cargarPartidosEnListWidget() {
    for (const auto& partido : partidos) {
        QString itemText = QString("Jornada: %1 | Liga: %2 | %3 vs %4 | %5 - %6 | Fecha: %7")
            .arg(QString::fromStdString(partido.getJornada()))
            .arg(QString::fromStdString(partido.getLiga()))
            .arg(QString::fromStdString(partido.getEquipoLocal()))
            .arg(QString::fromStdString(partido.getEquipoVisitante()))
            .arg(partido.getGolesLocal())
            .arg(partido.getGolesVisitante())
            .arg(QString::fromStdString(partido.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString() que devuelve una fecha legible.

        QListWidgetItem* item = new QListWidgetItem(itemText);

        // Guardamos un puntero al objeto Partido asociado al item
        item->setData(Qt::UserRole, QVariant::fromValue(partido));
        ui->listWidget->addItem(item);
    }
}

void MainWindow::limpiarListWidget(){
    ui->listWidget->clear();
}
void MainWindow::resetWidget(){
    limpiarListWidget();
    cargarPartidosEnListWidget();
}
void MainWindow::onItemSelectionChanged() {
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    ui->editButton->setEnabled(currentItem != nullptr);
    ui->deleteButton->setEnabled(currentItem != nullptr);
}

void MainWindow::on_eliminarButton_clicked() {
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    if (!currentItem) return;

    // Obtener los datos del partido seleccionado
    Partido partidoSeleccionado = currentItem->data(Qt::UserRole).value<Partido>();
    QString mensaje = QString("¿Está seguro de que desea eliminar el siguiente partido?\n\n"
                              "Jornada: %1\n"
                              "Liga: %2\n"
                              "Equipo Local: %3\n"
                              "Equipo Visitante: %4\n"
                              "Goles Local: %5\n"
                              "Goles Visitante: %6\n"
                              "Fecha: %7")
                          .arg(QString::fromStdString(partidoSeleccionado.getJornada()))
                          .arg(QString::fromStdString(partidoSeleccionado.getLiga()))
                          .arg(QString::fromStdString(partidoSeleccionado.getEquipoLocal()))
                          .arg(QString::fromStdString(partidoSeleccionado.getEquipoVisitante()))
                          .arg(partidoSeleccionado.getGolesLocal())
                          .arg(partidoSeleccionado.getGolesVisitante())
                          .arg(QString::fromStdString(partidoSeleccionado.getFecha().toString()));

    // Confirmar eliminación
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmar Eliminación", mensaje,
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // Llamar a la función para eliminar el partido de la base de datos
        DataControl dataControl;
        dataControl.eliminarPartido(rutaArchivo, partidoSeleccionado);
        servicioPartido.eliminarPartido(partidoSeleccionado);
        // Actualizar la interfaz
        partidos.clear();
        actualizarVectorDePartidos(rutaArchivo);
        limpiarListWidget();
        cargarPartidosEnListWidget();
        QMessageBox::information(this, "Aviso", "Partido eliminado correctamente");
    }
}

void MainWindow::on_editarButton_clicked() {
    QListWidgetItem* currentItem = ui->listWidget->currentItem();
    if (!currentItem) return;
    Partido partidoSeleccionado = currentItem->data(Qt::UserRole).value<Partido>();
    QString mensaje = QString("Detalles del partido seleccionado:\n"
                              "Jornada: %1\n"
                              "Liga: %2\n"
                              "Equipo Local: %3\n"
                              "Equipo Visitante: %4\n"
                              "Goles Local: %5\n"
                              "Goles Visitante: %6\n"
                              "Fecha: %7")
                          .arg(QString::fromStdString(partidoSeleccionado.getJornada()))
                          .arg(QString::fromStdString(partidoSeleccionado.getLiga()))
                          .arg(QString::fromStdString(partidoSeleccionado.getEquipoLocal()))
                          .arg(QString::fromStdString(partidoSeleccionado.getEquipoVisitante()))
                          .arg(partidoSeleccionado.getGolesLocal())
                          .arg(partidoSeleccionado.getGolesVisitante())
                          .arg(QString::fromStdString(partidoSeleccionado.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString()

    // Mostrar el MessageBox con la información del partido
    QMessageBox::information(this, "Partido Seleccionado", mensaje);

    Partido partidoOriginal(partidoSeleccionado.getEquipoLocalObj(),
                            partidoSeleccionado.getEquipoVisitanteObj(),
                            partidoSeleccionado.getGolesLocal(),
                            partidoSeleccionado.getGolesVisitante(),
                            partidoSeleccionado.getLiga(),
                            partidoSeleccionado.getFecha());
    partidoOriginal.setJornada(partidoSeleccionado.getJornada());
    QString jornada = QInputDialog::getText(this, "Editar Partido", "Nueva jornada:", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado.getJornada()));
    if (jornada.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo local no puede estar vacío.");
        return;
    }

    // Solicitar nuevo equipo local
    QString equipoLocal = QInputDialog::getText(this, "Editar Partido", "Nuevo Equipo Local:", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado.getEquipoLocal()));
    if (equipoLocal.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo local no puede estar vacío.");
        return;
    }

    // Solicitar nuevo equipo visitante
    QString equipoVisitante = QInputDialog::getText(this, "Editar Partido", "Nuevo Equipo Visitante:", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado.getEquipoVisitante()));
    if (equipoVisitante.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo visitante no puede estar vacío.");
        return;
    }

    // Solicitar nuevos goles del equipo local
    bool ok;
    int nuevosGolesLocal = QInputDialog::getInt(this, "Editar Partido", "Nuevos Goles Local:", partidoSeleccionado.getGolesLocal(), 0, 100, 1, &ok);
    if (!ok) return;  // El usuario canceló la entrada

    // Solicitar nuevos goles del equipo visitante
    int nuevosGolesVisitante = QInputDialog::getInt(this, "Editar Partido", "Nuevos Goles Visitante:", partidoSeleccionado.getGolesVisitante(), 0, 100, 1, &ok);
    if (!ok) return;  // El usuario canceló la entrada

    // Solicitar nueva fecha
    QString fechaStr = QInputDialog::getText(this, "Editar Partido", "Nueva Fecha (DD/MM/AAAA):", QLineEdit::Normal, QString::fromStdString(partidoSeleccionado.getFecha().toString()));
    if (fechaStr.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo de la fecha no puede estar vacío.");
        return;
    }

    // Validar y parsear la fecha ingresada
    QStringList fechaParts = fechaStr.split('/');
    if (fechaParts.size() != 3 || fechaParts[0].toInt(&ok) < 1 || fechaParts[0].toInt(&ok) > 31 ||
        fechaParts[1].toInt(&ok) < 1 || fechaParts[1].toInt(&ok) > 12 ||
        fechaParts[2].toInt(&ok) < 1900) {
        QMessageBox::warning(this, "Fecha Inválida", "Por favor, ingrese una fecha válida (DD/MM/AAAA).");
        return;
    }

    int dia = fechaParts[0].toInt();
    int mes = fechaParts[1].toInt();
    int anio = fechaParts[2].toInt();

    Fecha nuevaFecha(dia, mes, anio); // Asumiendo que tienes un constructor en Fecha que recibe dia, mes, año

    // Crear el objeto de partido editado
    Partido partidoEditado(Equipo(equipoLocal.toStdString()), Equipo(equipoVisitante.toStdString()),
                           nuevosGolesLocal, nuevosGolesVisitante, partidoSeleccionado.getLiga(), nuevaFecha);
    partidoEditado.setJornada(jornada.toStdString());

    servicioPartido.actualizarPartido(partidoOriginal.getLiga(), partidoEditado, partidoOriginal);
    // Llamar a la función para editar el partido en la base de datos
    DataControl dataControl;
    dataControl.editarPartido(rutaArchivo, partidoOriginal, partidoEditado);

    partidos.clear();
    actualizarVectorDePartidos(rutaArchivo);
    limpiarListWidget();
    cargarPartidosEnListWidget();
    QMessageBox::information(this, "Aviso", QString("Partido editado correctamente"));
}

void MainWindow::filtrarPartidosPorUmbral(ServicioPartidoTree& servicio){
    bool ok;
    int contadorIfs = 0;
    int umbral = QInputDialog::getInt(this, "Partidos por umbral", "Umbral de goles:", 0, 0, 100, 1, &ok);
    if (!ok) return;

    QStringList options;
    options << "Por encima del umbral" << "Por debajo del umbral";
    QString seleccion = QInputDialog::getItem(this, "Seleccionar opción", "Filtrar partidos:", options, 0, false, &ok);

    if (!ok) return;  // Si el usuario cancela, salimos

    bool porEncima = (seleccion == "Por encima");
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Partido> partidos = servicio.getPartidosPorUmbral(umbral, porEncima);
    contadorIfs += servicio.getContadorIfs();
    if (partidos.empty()) {
        QMessageBox::information(this, "Sin resultados", "No se encontraron partidos en la competición ingresada.");
        return;
    }

    limpiarListWidget();

    for (int i = 0; i <= partidos.size() - 1; ++i) {
        const auto& partido = partidos[i];

        QString itemText = QString("Jornada: %1 | Liga: %2 | %3 vs %4 | %5 - %6 | Fecha: %7")
                               .arg(QString::fromStdString(partido.getJornada()))
                               .arg(QString::fromStdString(partido.getLiga()))
                               .arg(QString::fromStdString(partido.getEquipoLocal()))
                               .arg(QString::fromStdString(partido.getEquipoVisitante()))
                               .arg(partido.getGolesLocal())
                               .arg(partido.getGolesVisitante())
                               .arg(QString::fromStdString(partido.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString()

        QListWidgetItem* item = new QListWidgetItem(itemText);

        item->setData(Qt::UserRole, QVariant::fromValue(partido));
        ui->listWidget->addItem(item);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(servicio.getContadorIfs() + contadorIfs));

}


void MainWindow::mostrarTop5Partidos(ServicioPartidoTree& servicio){
    bool ok;
    int contadorIfs = 0;
    QString competicion = QInputDialog::getText(this, "Ingrese Competición", "Competición:", QLineEdit::Normal, "", &ok);

    if (!ok || competicion.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ninguna competición.");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    try {
        ArbolBinarioAVL<Partido> partidos = servicio.getPartidos(competicion.toStdString());
        std::vector<Partido> partidosLista = partidos.inorder();  // Obtener partidos en orden ascendente

        if (partidosLista.empty()) {
            QMessageBox::information(this, "Sin resultados", "No se encontraron partidos en la competición ingresada.");
            return;
        }

        // Limpiar el `listWidget` antes de cargar los nuevos partidos
        limpiarListWidget();

        // Mostrar los 5 partidos con más goles al final de la lista ordenada
        int startIdx = std::max(0, static_cast<int>(partidosLista.size()) - 5);
        for (int i = partidosLista.size() - 1; i >= startIdx; --i) {
            const auto& partido = partidosLista[i];

            QString itemText = QString("Jornada: %1 | Liga: %2 | %3 vs %4 | %5 - %6 | Fecha: %7")
                                   .arg(QString::fromStdString(partido.getJornada()))
                                   .arg(QString::fromStdString(partido.getLiga()))
                                   .arg(QString::fromStdString(partido.getEquipoLocal()))
                                   .arg(QString::fromStdString(partido.getEquipoVisitante()))
                                   .arg(partido.getGolesLocal())
                                   .arg(partido.getGolesVisitante())
                                   .arg(QString::fromStdString(partido.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString()

            QListWidgetItem* item = new QListWidgetItem(itemText);

            item->setData(Qt::UserRole, QVariant::fromValue(partido));
            ui->listWidget->addItem(item);
        }
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar la competición.");
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(servicio.getContadorIfs()));
}

void MainWindow::mostrarGolesTotalesPorEquipo(ServicioPartidoTree& servicio) {
    // Pedir al usuario que ingrese el nombre del equipo
    bool ok;
    int contadorIfs = 0;
    QString equipoNombre = QInputDialog::getText(this, "Ingrese Nombre del Equipo", "Equipo:", QLineEdit::Normal, "", &ok);

    if (!ok || equipoNombre.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ningún equipo.");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration;
    try {
        bool equipoEncontrado = false;
        QString resultados;

        // Buscar el equipo en todas las competiciones
        for (const auto& [competicion, equiposEnCompeticion] : servicio.getEquipos()) {
            auto it = equiposEnCompeticion.find(equipoNombre.toStdString());
            if (it != equiposEnCompeticion.end()) {
                equipoEncontrado = true;
                const Equipo& equipo = it->second;
                Estadisticas stats = equipo.getEstadisticas(competicion);

                resultados += QString("Competición: %1\nEquipo: %2\nGoles a favor: %3\nGoles en contra: %4\nPartidos jugados: %5\n\n")
                                  .arg(QString::fromStdString(competicion))
                                  .arg(QString::fromStdString(equipo.getNombre()))
                                  .arg(stats.golesAFavor)
                                  .arg(stats.golesEnContra)
                                  .arg(stats.partidosJugados);
            } contadorIfs++;
        }
        auto end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        if (!equipoEncontrado) {
            QMessageBox::information(this, "Equipo no encontrado", "El equipo no fue encontrado en ninguna competición.");
        } else {
            // Mostrar los resultados en un cuadro de mensaje
            QMessageBox::information(this, "Estadísticas del Equipo", resultados);
        }
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar el equipo.");
    }



    // Mostrar el tiempo de ejecución
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(servicio.getContadorIfs() + contadorIfs));
}

void MainWindow::mostrarPromedioGolesPorEquipo(ServicioPartidoTree& servicio) {
    // Solicitar al usuario el nombre del equipo
    bool ok;
    int contadorIfs = 0;
    QString equipoNombre = QInputDialog::getText(this, "Ingrese Nombre del Equipo", "Equipo:", QLineEdit::Normal, "", &ok);

    if (!ok || equipoNombre.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ningún equipo.");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration;
    try {
        bool equipoEncontrado = false;
        QString resultados;

        // Buscar el equipo en todas las competiciones
        for (const auto& [competicion, equiposEnCompeticion] : servicio.getEquipos()) {
            auto it = equiposEnCompeticion.find(equipoNombre.toStdString());
            if (it != equiposEnCompeticion.end()) {
                equipoEncontrado = true;
                const Equipo& equipo = it->second;
                Estadisticas stats = equipo.getEstadisticas(competicion);

                float promedioFavor = stats.partidosJugados > 0 ? static_cast<float>(stats.golesAFavor) / stats.partidosJugados : 0;
                float promedioContra = stats.partidosJugados > 0 ? static_cast<float>(stats.golesEnContra) / stats.partidosJugados : 0;

                resultados += QString("Competición: %1\nEquipo: %2\nPromedio de goles a favor: %3\nPromedio de goles en contra: %4\n\n")
                                  .arg(QString::fromStdString(competicion))
                                  .arg(QString::fromStdString(equipo.getNombre()))
                                  .arg(promedioFavor, 0, 'f', 2)
                                  .arg(promedioContra, 0, 'f', 2);
            } contadorIfs++;
        }
        auto end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        if (!equipoEncontrado) {
            QMessageBox::information(this, "Equipo no encontrado", "El equipo no fue encontrado en ninguna competición.");
        } else {
            // Mostrar los resultados en un cuadro de mensaje
            QMessageBox::information(this, "Promedio de Goles por Equipo", resultados);
        }
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar el equipo.");
    }



    // Mostrar el tiempo de ejecución
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(servicio.getContadorIfs() + contadorIfs));
}

void MainWindow::mostrarVictoriasYDerrotasPorCompeticion(ServicioPartidoTree& servicio) {
    // Solicitar el nombre del equipo al usuario
    bool ok;
    int contadorIfs = 0;
    QString equipoNombre = QInputDialog::getText(this, "Ingrese Nombre del Equipo", "Equipo:", QLineEdit::Normal, "", &ok);

    if (!ok || equipoNombre.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ningún equipo.");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration;
    try {
        bool equipoEncontrado = false;
        QString resultados;

        // Buscar el equipo en todas las competiciones
        for (const auto& [competicion, equiposEnCompeticion] : servicio.getEquipos()) {
            auto it = equiposEnCompeticion.find(equipoNombre.toStdString());
            if (it != equiposEnCompeticion.end()) {
                equipoEncontrado = true;
                const Equipo& equipo = it->second;
                Estadisticas stats = equipo.getEstadisticas(competicion);

                resultados += QString("Competición: %1\nEquipo: %2\nVictorias: %3\nDerrotas: %4\n\n")
                                  .arg(QString::fromStdString(competicion))
                                  .arg(QString::fromStdString(equipo.getNombre()))
                                  .arg(stats.victorias)
                                  .arg(stats.derrotas);
            } contadorIfs++;
        }
        auto end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        if (!equipoEncontrado) {
            QMessageBox::information(this, "Equipo no encontrado", "El equipo no fue encontrado en ninguna competición.");
        } else {
            // Mostrar los resultados en un cuadro de mensaje
            QMessageBox::information(this, "Victorias y Derrotas por Competición", resultados);
        }
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar el equipo.");
    }



    // Mostrar el tiempo de ejecución
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(servicio.getContadorIfs() + contadorIfs));
}


void MainWindow::mostrarFechaConMasYMenosGoles(ServicioPartidoTree& servicio) {
    // Solicitar la competición
    bool ok;
    QString competicion = QInputDialog::getText(this, "Ingrese la Competición", "Competición:", QLineEdit::Normal, "", &ok);
    if (!ok || competicion.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ninguna competición.");
        return;
    }

    // Solicitar el nombre del equipo
    QString equipoNombre = QInputDialog::getText(this, "Ingrese Nombre del Equipo", "Equipo:", QLineEdit::Normal, "", &ok);
    if (!ok || equipoNombre.isEmpty()) {
        QMessageBox::warning(this, "Error", "No se ingresó ningún equipo.");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration;
    try {
        // Obtener el partido con la fecha con más goles
        Partido partidoMasGoles = servicio.getFechaConMasGoles(equipoNombre.toStdString(), competicion.toStdString());
        QString resultadoMasGoles = QString("Fecha con más goles:\nEquipo Local: %1\nEquipo Visitante: %2\nGoles: %3 - %4\nFecha: %5\n\n")
                                        .arg(QString::fromStdString(partidoMasGoles.getEquipoLocal()))
                                        .arg(QString::fromStdString(partidoMasGoles.getEquipoVisitante()))
                                        .arg(partidoMasGoles.getGolesLocal())
                                        .arg(partidoMasGoles.getGolesVisitante())
                                        .arg(QString::fromStdString(partidoMasGoles.getFecha().toString()));

        // Obtener el partido con la fecha con menos goles
        Partido partidoMenosGoles = servicio.getFechaConMenosGoles(equipoNombre.toStdString(), competicion.toStdString());
        QString resultadoMenosGoles = QString("Fecha con menos goles:\nEquipo Local: %1\nEquipo Visitante: %2\nGoles: %3 - %4\nFecha: %5\n\n")
                                          .arg(QString::fromStdString(partidoMenosGoles.getEquipoLocal()))
                                          .arg(QString::fromStdString(partidoMenosGoles.getEquipoVisitante()))
                                          .arg(partidoMenosGoles.getGolesLocal())
                                          .arg(partidoMenosGoles.getGolesVisitante())
                                          .arg(QString::fromStdString(partidoMenosGoles.getFecha().toString()));
        auto end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        // Mostrar los resultados en un QMessageBox
        QMessageBox::information(this, "Resultados", resultadoMasGoles + "\n" + resultadoMenosGoles);
    } catch (...) {
        QMessageBox::warning(this, "Error", "No se pudo recuperar la competición o el equipo.");
    }



    // Mostrar el tiempo de ejecución
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(servicio.getContadorIfs()));
}

void MainWindow::mostrarEquiposConMasYMenosGoles(ServicioPartidoTree& servicio) {
    // Solicitar al usuario el nombre de la competición
    QString competicion = QInputDialog::getText(this, "Competición", "Ingrese el nombre de la competición (o déjelo en blanco para todas):");
    int contadorIfs = 0;
    auto start = std::chrono::high_resolution_clock::now();  // Inicio del cronómetro

    std::pair<Equipo, int> equipoMasGoles;
    std::pair<Equipo, int> equipoMenosGoles;


    if (competicion.isEmpty()) {
        // Si la competición está en blanco, obtener los equipos con más y menos goles en todas las competiciones
        equipoMasGoles = servicio.obtenerEquipoConMasGoles();
        contadorIfs+=servicio.getContadorIfs();
        equipoMenosGoles = servicio.obtenerEquipoConMenosGoles();
        contadorIfs+=servicio.getContadorIfs();
    } else {
        // Obtener los equipos con más y menos goles en la competición especificada
        equipoMasGoles = servicio.obtenerEquipoConMasGolesEnCompeticion(competicion.toStdString());
        equipoMenosGoles = servicio.obtenerEquipoConMenosGolesEnCompeticion(competicion.toStdString());
    }

    if(equipoMasGoles.first.getNombre() == "" || equipoMenosGoles.first.getNombre() == ""){
        QMessageBox::warning(this, "Error", "No se encontro la competicion.");
        return;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Preparar el mensaje a mostrar
    QString resultado = competicion.isEmpty()
                            ? "En todas las competiciones:\n\n"
                            : QString("En la competición %1:\n\n").arg(competicion);

    resultado += QString("Equipo con más goles: %1 con %2 goles.\n"
                         "Equipo con menos goles: %3 con %4 goles.")
                     .arg(QString::fromStdString(equipoMasGoles.first.getNombre()))
                     .arg(equipoMasGoles.second)
                     .arg(QString::fromStdString(equipoMenosGoles.first.getNombre()))
                     .arg(equipoMenosGoles.second);


    // Mostrar el resultado en un cuadro de diálogo
    QMessageBox::information(this, "Resultados", resultado);
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(contadorIfs));
}

void MainWindow::compararRendimientoEquipos(ServicioPartidoTree& servicio) {
    QString equipo1 = QInputDialog::getText(this, "Equipo 1", "Ingrese el nombre del primer equipo:");
    QString equipo2 = QInputDialog::getText(this, "Equipo 2", "Ingrese el nombre del segundo equipo:");

    if (equipo1.isEmpty() || equipo2.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debe ingresar nombres válidos para ambos equipos.");
        return;
    }
    auto start = std::chrono::high_resolution_clock::now();  // Inicio del cronómetro
    std::chrono::duration<double> duration;
    // Obtener partidos entre los dos equipos
    std::vector<Partido> partidos = servicio.obtenerPartidosEntreEquipos(equipo1.toStdString(), equipo2.toStdString());
    int victoriasEquipo1 = 0, victoriasEquipo2 = 0, empates = 0;

    limpiarListWidget();
    int startIdx = std::max(0, static_cast<int>(partidos.size()) - 5);
    for (int i = partidos.size() - 1; i >= startIdx; --i) {
        const auto& partido = partidos[i];

        QString itemText = QString("Jornada: %1 | Liga: %2 | %3 vs %4 | %5 - %6 | Fecha: %7")
                               .arg(QString::fromStdString(partido.getJornada()))
                               .arg(QString::fromStdString(partido.getLiga()))
                               .arg(QString::fromStdString(partido.getEquipoLocal()))
                               .arg(QString::fromStdString(partido.getEquipoVisitante()))
                               .arg(partido.getGolesLocal())
                               .arg(partido.getGolesVisitante())
                               .arg(QString::fromStdString(partido.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString()

        QListWidgetItem* item = new QListWidgetItem(itemText);

        item->setData(Qt::UserRole, QVariant::fromValue(partido));
        ui->listWidget->addItem(item);
    }

    for (const auto& partido : partidos) {
        if (partido.getGolesLocal() == partido.getGolesVisitante()) {
            empates++;
        } else if ((partido.getEquipoLocal() == equipo1.toStdString() && partido.getGolesLocal() > partido.getGolesVisitante()) ||
                   (partido.getEquipoVisitante() == equipo1.toStdString() && partido.getGolesVisitante() > partido.getGolesLocal())) {
            victoriasEquipo1++;
        } else {
            victoriasEquipo2++;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();  // Fin del cronómetro
    duration = end - start;
    // Mostrar resultados
    QString resultado = QString("Partidos jugados: %1\nEmpates: %2\n")
                            .arg(partidos.size())
                            .arg(empates);
    resultado += QString("%1 ganó %2 partidos.\n").arg(equipo1).arg(victoriasEquipo1);
    resultado += QString("%1 ganó %2 partidos.\n").arg(equipo2).arg(victoriasEquipo2);

    QMessageBox::information(this, "Resultados de la Comparación", resultado);
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(servicio.getContadorIfs()));

}

void MainWindow::competicionConMasGoles(ServicioPartidoTree& servicio) {
    auto start = std::chrono::high_resolution_clock::now();  // Inicio del cronómetro
    int contadorIfs = 0;
    // Obtenemos la competición con más goles y la cantidad de goles
    auto [competicionMasGoles, totalGoles] = servicio.obtenerCompeticionConMasGoles();
    contadorIfs += servicio.getContadorIfs();
    auto[competicionMenosGoles, totalMinGoles] = servicio.obtenerCompeticionConMenosGoles();
    contadorIfs += servicio.getContadorIfs();
    auto end = std::chrono::high_resolution_clock::now();  // Fin del cronómetro
    std::chrono::duration<double> duration = end - start;

    // Mostrar el resultado en un mensaje
    QString resultado = QString("La competición con más goles es: %1 con %2 goles convertidos.")
                            .arg(QString::fromStdString(competicionMasGoles))
                            .arg(totalGoles);

    resultado += QString("\n\nLa competición con menos goles es: %1 con %2 goles convertidos.")
                     .arg(QString::fromStdString(competicionMenosGoles))
                     .arg(totalMinGoles);
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(contadorIfs));
    QMessageBox::information(this, "Resultados", resultado);
}

void MainWindow::buscarPartidosEntreFechasButton(ServicioPartidoTree& servicio) {
    // Solicitar el nombre del equipo
    QString equipo = QInputDialog::getText(this, "Buscar Partidos", "Ingrese el nombre del equipo:");
    if (equipo.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo del equipo no puede estar vacío.");
        return;
    }

    // Solicitar la primera fecha (fecha de inicio)
    QString fechaInicioStr = QInputDialog::getText(this, "Buscar Partidos", "Ingrese la fecha de inicio (DD/MM/AAAA):");
    if (fechaInicioStr.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "La fecha de inicio no puede estar vacía.");
        return;
    }

    // Validar y convertir la primera fecha
    QStringList fechaInicioParts = fechaInicioStr.split('/');
    bool ok;
    if (fechaInicioParts.size() != 3 || fechaInicioParts[0].toInt(&ok) < 1 || fechaInicioParts[0].toInt(&ok) > 31 ||
        fechaInicioParts[1].toInt(&ok) < 1 || fechaInicioParts[1].toInt(&ok) > 12 ||
        fechaInicioParts[2].toInt(&ok) < 1900) {
        QMessageBox::warning(this, "Fecha Inválida", "Por favor, ingrese una fecha válida (DD/MM/AAAA) para la fecha de inicio.");
        return;
    }
    Fecha fechaInicio(fechaInicioParts[0].toInt(), fechaInicioParts[1].toInt(), fechaInicioParts[2].toInt());

    // Solicitar la segunda fecha (fecha de fin)
    QString fechaFinStr = QInputDialog::getText(this, "Buscar Partidos", "Ingrese la fecha de fin (DD/MM/AAAA):");
    if (fechaFinStr.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "La fecha de fin no puede estar vacía.");
        return;
    }

    // Validar y convertir la segunda fecha
    QStringList fechaFinParts = fechaFinStr.split('/');
    if (fechaFinParts.size() != 3 || fechaFinParts[0].toInt(&ok) < 1 || fechaFinParts[0].toInt(&ok) > 31 ||
        fechaFinParts[1].toInt(&ok) < 1 || fechaFinParts[1].toInt(&ok) > 12 ||
        fechaFinParts[2].toInt(&ok) < 1900) {
        QMessageBox::warning(this, "Fecha Inválida", "Por favor, ingrese una fecha válida (DD/MM/AAAA) para la fecha de fin.");
        return;
    }
    Fecha fechaFin(fechaFinParts[0].toInt(), fechaFinParts[1].toInt(), fechaFinParts[2].toInt());

    // Verificar que la fecha de inicio no sea mayor que la fecha de fin
    if (fechaInicio > fechaFin) {
        QMessageBox::warning(this, "Rango de Fechas Inválido", "La fecha de inicio no puede ser mayor que la fecha de fin.");
        return;
    }

    // Llamar a la función de servicio para obtener los partidos entre las fechas
    auto start = std::chrono::high_resolution_clock::now();
    std::string equipoStr = equipo.toStdString();
    std::vector<Partido> partidosEntreFechas = servicio.getPartidosEntreFechas(equipoStr, fechaInicio, fechaFin);
    auto end = std::chrono::high_resolution_clock::now();  // Fin del cronómetro
    std::chrono::duration<double> duration = end - start;
    // Limpiar el ListWidget antes de cargar los nuevos resultados
    ui->listWidget->clear();

    // Mostrar los partidos en el ListWidget
    for (const auto& partido : partidosEntreFechas) {
        QString itemText = QString("Jornada: %1 | Liga: %2 | %3 vs %4 | %5 - %6 | Fecha: %7")
        .arg(QString::fromStdString(partido.getJornada()))
            .arg(QString::fromStdString(partido.getLiga()))
            .arg(QString::fromStdString(partido.getEquipoLocal()))
            .arg(QString::fromStdString(partido.getEquipoVisitante()))
            .arg(partido.getGolesLocal())
            .arg(partido.getGolesVisitante())
            .arg(QString::fromStdString(partido.getFecha().toString()));  // Suponiendo que Fecha tiene un método toString()

        QListWidgetItem* item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, QVariant::fromValue(partido));
        ui->listWidget->addItem(item);
    }
    QMessageBox::information(this, "Tiempo de ejecución", QString("Tiempo transcurrido: %1 segundos \nCantidad de if: %2")
                                                              .arg(duration.count())
                                                              .arg(servicio.getContadorIfs()));

    // Mostrar un mensaje si no se encontraron partidos
    if (partidosEntreFechas.empty()) {
        QMessageBox::information(this, "Sin Resultados", "No se encontraron partidos para los criterios especificados.");
    }
}

void MainWindow::on_agregarButton_clicked() {
    // Solicitar jornada
    QString jornada = QInputDialog::getText(this, "Agregar Partido", "Jornada:");
    if (jornada.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo de jornada no puede estar vacío.");
        return;
    }

    // Solicitar equipo local
    QString equipoLocal = QInputDialog::getText(this, "Agregar Partido", "Equipo Local:");
    if (equipoLocal.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo de equipo local no puede estar vacío.");
        return;
    }

    // Solicitar equipo visitante
    QString equipoVisitante = QInputDialog::getText(this, "Agregar Partido", "Equipo Visitante:");
    if (equipoVisitante.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo de equipo visitante no puede estar vacío.");
        return;
    }

    // Solicitar goles del equipo local
    bool ok;
    int golesLocal = QInputDialog::getInt(this, "Agregar Partido", "Goles Local:", 0, 0, 100, 1, &ok);
    if (!ok) return; // El usuario canceló la entrada

    // Solicitar goles del equipo visitante
    int golesVisitante = QInputDialog::getInt(this, "Agregar Partido", "Goles Visitante:", 0, 0, 100, 1, &ok);
    if (!ok) return; // El usuario canceló la entrada

    // Solicitar fecha
    QString fechaStr = QInputDialog::getText(this, "Agregar Partido", "Fecha (DD/MM/AAAA):");
    if (fechaStr.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo de fecha no puede estar vacío.");
        return;
    }

    QStringList fechaParts = fechaStr.split('/');
    if (fechaParts.size() != 3 || fechaParts[0].toInt(&ok) < 1 || fechaParts[0].toInt(&ok) > 31 ||
        fechaParts[1].toInt(&ok) < 1 || fechaParts[1].toInt(&ok) > 12 ||
        fechaParts[2].toInt(&ok) < 1900) {
        QMessageBox::warning(this, "Fecha Inválida", "Por favor, ingrese una fecha válida (DD/MM/AAAA).");
        return;
    }

    int dia = fechaParts[0].toInt();
    int mes = fechaParts[1].toInt();
    int anio = fechaParts[2].toInt();

    Fecha fecha(dia, mes, anio); // Crear objeto Fecha

    // Solicitar liga
    QString liga = QInputDialog::getText(this, "Agregar Partido", "Liga:");
    if (liga.isEmpty()) {
        QMessageBox::warning(this, "Entrada Vacía", "El campo de liga no puede estar vacío.");
        return;
    }

    // Crear el objeto Partido
    Partido nuevoPartido(Equipo(equipoLocal.toStdString()), Equipo(equipoVisitante.toStdString()),
                         golesLocal, golesVisitante, liga.toStdString(), fecha);
    nuevoPartido.setJornada(jornada.toStdString());

    // Guardar el partido en el archivo CSV
    DataControl dataControl;
    dataControl.agregarPartido(rutaArchivo, nuevoPartido, jornada.toStdString());

    partidos.push_back(nuevoPartido);
    servicioPartido.registrarPartidoEnHash(nuevoPartido);
    servicioPartido.actualizarEstadisticasPorCompeticion(nuevoPartido);
    limpiarListWidget();
    cargarPartidosEnListWidget();
    QMessageBox::information(this, "Aviso", "Partido agregado correctamente");
}


MainWindow::~MainWindow()
{
    delete ui;
}
