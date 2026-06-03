#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <clocale>
using namespace std;

struct Usuario
{
    int id;
    string nombre;
    string correo;
    string contrasena;
    string direccion;
    string metodoPago;
};

struct Producto
{
    int id;
    string nombre;
    string descripcion;
    double precio;
    int stock;
};

struct ItemCarrito
{
    Producto producto;
    int cantidad;
};

struct Comentario
{
    int id;
    string producto;
    string usuario;
    string texto;
    string fecha;
};

struct ProductoVendido
{
    int id;
    string nombre;
    int cantidadTotal;
    vector<string> ordenes;
};

// punto 2
struct OrdenDeCompra
{
    int idOrden;
    Usuario usuario;
    vector<ItemCarrito> productos;
    double subtotal;
    double impuestos;
    double envio;
    double total;
    double descuentoBono; // punto 4c: descuento aplicado por bono
};
// punto 2

// punto 4b: estructura para guardar bonos de usuarios
struct BonoUsuario
{
    int idUsuario;
    double porcentajeDescuento; // 0.50 = 50%
    bool usado;
};

int convertirEntero(const string &texto)
{
    int valor = 0;
    stringstream ss(texto);
    ss >> valor;
    return valor;
}

double convertirDouble(const string &texto)
{
    double valor = 0;
    stringstream ss(texto);
    ss >> valor;
    return valor;
}

vector<Usuario> cargarUsuarios(const string &nombreArchivo)
{
    vector<Usuario> usuarios;
    ifstream archivo(nombreArchivo.c_str());
    string linea;
    getline(archivo, linea);

    while (getline(archivo, linea))
    {
        stringstream ss(linea);
        Usuario u;
        string idStr;
        getline(ss, idStr, ',');
        getline(ss, u.nombre, ',');
        getline(ss, u.correo, ',');
        getline(ss, u.contrasena, ',');
        getline(ss, u.direccion, ',');
        getline(ss, u.metodoPago, ',');
        u.id = convertirEntero(idStr);
        usuarios.push_back(u);
    }
    return usuarios;
}

vector<Producto> cargarProductos(const string &nombreArchivo)
{
    vector<Producto> productos;
    ifstream archivo(nombreArchivo.c_str());
    string linea;
    getline(archivo, linea);

    while (getline(archivo, linea))
    {
        stringstream ss(linea);
        Producto p;
        string idStr, precioStr, stockStr;
        getline(ss, idStr, ',');
        getline(ss, p.nombre, ',');
        getline(ss, p.descripcion, ',');
        getline(ss, precioStr, ',');
        getline(ss, stockStr, ',');
        p.id = convertirEntero(idStr);
        p.precio = convertirDouble(precioStr);
        p.stock = convertirEntero(stockStr);
        productos.push_back(p);
    }
    return productos;
}

vector<Comentario> cargarComentarios(const string &nombreArchivo)
{
    vector<Comentario> comentarios;
    ifstream archivo(nombreArchivo.c_str());
    string linea;

    if (!archivo.is_open())
    {
        cout << "Error: No se pudo abrir " << nombreArchivo << endl;
        return comentarios;
    }

    getline(archivo, linea);

    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        stringstream ss(linea);
        Comentario c;
        string idStr;

        getline(ss, idStr, ',');
        getline(ss, c.producto, ',');
        getline(ss, c.usuario, ',');
        getline(ss, c.texto, ',');
        getline(ss, c.fecha, ',');

        c.id = convertirEntero(idStr);
        comentarios.push_back(c);
    }

    archivo.close();
    return comentarios;
}

Usuario *iniciarSesion(vector<Usuario> &usuarios, const string &correo, const string &contrasena)
{
    for (size_t i = 0; i < usuarios.size(); ++i)
        if (usuarios[i].correo == correo && usuarios[i].contrasena == contrasena)
            return &usuarios[i];
    return NULL;
}

vector<Producto> listarProductosBajoStock(const vector<Producto> &productos)
{
    vector<Producto> bajos;
    for (size_t i = 0; i < productos.size(); ++i)
        if (productos[i].stock < 15)
            bajos.push_back(productos[i]);
    return bajos;
}
int obtenerUltimoIdCarrito()
{
    ifstream archivo("Carrito.txt");
    if (!archivo)
        return 0;

    string linea;
    int ultimoId = 0;

    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        stringstream ss(linea);
        string idCarritoStr;

        getline(ss, idCarritoStr, ',');

        int idCarrito = convertirEntero(idCarritoStr);

        if (idCarrito > ultimoId)
            ultimoId = idCarrito;
    }

    return ultimoId;
}

int contarCarritosUsuario(int idUsuario)
{
    ifstream archivo("Carrito.txt");

    if (!archivo)
        return 0;

    vector<int> idsCarritos;
    string linea;

    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        stringstream ss(linea);

        string idCarritoStr;
        string idUsuarioStr;

        getline(ss, idCarritoStr, ',');
        getline(ss, idUsuarioStr, ',');

        int idCarrito = convertirEntero(idCarritoStr);
        int usuario = convertirEntero(idUsuarioStr);

        if (usuario == idUsuario)
        {

            bool existe = false;

            for (size_t i = 0; i < idsCarritos.size(); i++)
            {
                if (idsCarritos[i] == idCarrito)
                {
                    existe = true;
                    break;
                }
            }

            if (!existe)
                idsCarritos.push_back(idCarrito);
        }
    }

    return idsCarritos.size();
}

void guardarCarritoArchivo(const Usuario &usuario,
                           const vector<ItemCarrito> &carrito)
{

    if (carrito.empty())
    {
        cout << "El carrito esta vacio.\n";
        return;
    }

    if (contarCarritosUsuario(usuario.id) >= 2)
    {
        cout << "\nERROR: El usuario ya tiene 2 carritos registrados.\n";
        return;
    }

    int idCarrito = obtenerUltimoIdCarrito() + 1;

    ofstream archivo("Carrito.txt", ios::app);

    for (size_t i = 0; i < carrito.size(); i++)
    {

        double subtotal =
            carrito[i].cantidad *
            carrito[i].producto.precio;

        archivo << idCarrito << ","
                << usuario.id << ","
                << usuario.nombre << ","
                << carrito[i].producto.id << ","
                << carrito[i].producto.nombre << ","
                << carrito[i].cantidad << ","
                << carrito[i].producto.precio << ","
                << subtotal << "\n";
    }

    archivo.close();

    cout << "\nCarrito #" << idCarrito << " guardado correctamente.\n";
}

void mostrarCarritoArchivo()
{
    ifstream archivo("Carrito.txt");
    if (!archivo)
    {
        cout << "No hay carrito registrado.\n";
        return;
    }

    string linea;
    cout << "\n=== CONTENIDO DEL CARRITO GUARDADO ===\n";
    cout << "Usuario | Producto | Cantidad | Precio | Subtotal\n";
    while (getline(archivo, linea))
    {
        stringstream ss(linea);
        string idCarrito;
        string idUsuario;
        string nombreUsuario;
        string idProd;
        string nombreProd;
        string cantidad;
        string precio;
        string subtotal;

        getline(ss, idCarrito, ',');
        getline(ss, idUsuario, ',');
        getline(ss, nombreUsuario, ',');
        getline(ss, idProd, ',');
        getline(ss, nombreProd, ',');
        getline(ss, cantidad, ',');
        getline(ss, precio, ',');
        getline(ss, subtotal, ',');

        cout << "Carrito #" << idCarrito
             << " | Usuario: " << nombreUsuario
             << " | Producto: " << nombreProd
             << " | Cantidad: " << cantidad
             << " | Precio: $" << precio
             << " | Subtotal: $" << subtotal
             << "\n";
    }
    archivo.close();
}

void mostrarCarrito(const vector<ItemCarrito> &carrito)
{
    cout << "\n=== CARRITO DE COMPRAS ===\n";
    double total = 0;
    for (size_t i = 0; i < carrito.size(); ++i)
    {
        double subtotal = carrito[i].cantidad * carrito[i].producto.precio;
        cout << carrito[i].producto.nombre
             << " (Precio: $" << carrito[i].producto.precio
             << ", Cantidad: " << carrito[i].cantidad
             << ") = $" << subtotal << "\n";
        total += subtotal;
    }
    cout << "TOTAL: $" << total << "\n";
}

void mostrarComentariosDesdeFecha(const vector<Comentario> &comentarios, const string &fechaLimite)
{
    cout << "\n=== COMENTARIOS DESDE EL " << fechaLimite << " EN ADELANTE ===\n";
    bool encontrado = false;

    for (size_t i = 0; i < comentarios.size(); ++i)
    {
        if (comentarios[i].fecha >= fechaLimite)
        {
            cout << "[" << comentarios[i].fecha << "] "
                 << "Producto: " << comentarios[i].producto << " | "
                 << "Usuario: " << comentarios[i].usuario << "\n"
                 << "Comentario: " << comentarios[i].texto << "\n";
            cout << "------------------------------------------\n";
            encontrado = true;
        }
    }

    if (!encontrado)
    {
        cout << "No se encontraron comentarios despu?s de esa fecha.\n";
    }
}
void listarProductosCarritoPorUsuario()
{

    ifstream archivo("Carrito.txt");

    if (!archivo)
    {
        cout << "No existe el archivo de carritos.\n";
        return;
    }

    string linea;

    cout << "\n=== PRODUCTOS DEL CARRITO POR USUARIO ===\n";

    while (getline(archivo, linea))
    {

        stringstream ss(linea);

        string idCarrito;
        string idUsuario;
        string nombreUsuario;
        string idProducto;
        string nombreProducto;
        string cantidad;
        string precio;
        string subtotal;

        getline(ss, idCarrito, ',');
        getline(ss, idUsuario, ',');
        getline(ss, nombreUsuario, ',');
        getline(ss, idProducto, ',');
        getline(ss, nombreProducto, ',');
        getline(ss, cantidad, ',');
        getline(ss, precio, ',');
        getline(ss, subtotal, ',');
        cout << "Carrito #" << idCarrito
             << " | Usuario: " << nombreUsuario
             << " | Producto: " << nombreProducto
             << " | Cantidad: " << cantidad
             << " | Precio: $" << precio
             << endl;
    }

    archivo.close();
}

void listarProductosMasVendidos()
{
    ifstream archivo("Carrito.txt");
    if (!archivo)
    {
        cout << "\nNo se encontraron registros de ventas (Carrito.txt no existe).\n";
        return;
    }

    vector<ProductoVendido> productosVendidos;
    string linea;

    while (getline(archivo, linea))
    {
        if (linea.empty())
            continue;

        stringstream ss(linea);
        string idCarrito, idUsuario, nombreUsuario, idProdStr, nombreProd, cantidadStr, precio, subtotal;

        getline(ss, idCarrito, ',');
        getline(ss, idUsuario, ',');
        getline(ss, nombreUsuario, ',');
        getline(ss, idProdStr, ',');
        getline(ss, nombreProd, ',');
        getline(ss, cantidadStr, ',');
        getline(ss, precio, ',');
        getline(ss, subtotal);

        if (!subtotal.empty() && subtotal[subtotal.size() - 1] == '\r')
        {
            subtotal.erase(subtotal.size() - 1);
        }

        int idProd = convertirEntero(idProdStr);
        int cantidad = convertirEntero(cantidadStr);

        bool encontrado = false;
        for (size_t i = 0; i < productosVendidos.size(); ++i)
        {
            if (productosVendidos[i].id == idProd)
            {
                productosVendidos[i].cantidadTotal += cantidad;
                string detalleOrden = "Cliente: " + nombreUsuario + " (ID: " + idUsuario + ") - Cantidad: " + cantidadStr + " - Subtotal: $" + subtotal;
                productosVendidos[i].ordenes.push_back(detalleOrden);
                encontrado = true;
                break;
            }
        }

        if (!encontrado)
        {
            ProductoVendido pv;
            pv.id = idProd;
            pv.nombre = nombreProd;
            pv.cantidadTotal = cantidad;
            string detalleOrden = "Cliente: " + nombreUsuario + " (ID: " + idUsuario + ") - Cantidad: " + cantidadStr + " - Subtotal: $" + subtotal;
            pv.ordenes.push_back(detalleOrden);
            productosVendidos.push_back(pv);
        }
    }
    archivo.close();

    for (size_t i = 0; i < productosVendidos.size(); ++i)
    {
        for (size_t j = i + 1; j < productosVendidos.size(); ++j)
        {
            if (productosVendidos[j].cantidadTotal > productosVendidos[i].cantidadTotal)
            {
                ProductoVendido temporal = productosVendidos[i];
                productosVendidos[i] = productosVendidos[j];
                productosVendidos[j] = temporal;
            }
        }
    }

    cout << "\n=======================================================\n";
    cout << "          REPORTE DE PRODUCTOS MAS VENDIDOS\n";
    cout << "=======================================================\n";

    if (productosVendidos.empty())
    {
        cout << "No hay ventas registradas en el archivo.\n";
        return;
    }

    for (size_t i = 0; i < productosVendidos.size(); ++i)
    {
        cout << i + 1 << ". Producto ID: " << productosVendidos[i].id << "\n";
        cout << "   Nombre: " << productosVendidos[i].nombre << "\n";
        cout << "   Cantidad Total Vendida: " << productosVendidos[i].cantidadTotal << " unidades\n";
        cout << "   Ordenes de compra asociadas:\n";
        for (size_t j = 0; j < productosVendidos[i].ordenes.size(); ++j)
        {
            cout << "     [Orden " << j + 1 << "] " << productosVendidos[i].ordenes[j] << "\n";
        }
        cout << "-------------------------------------------------------\n";
    }
}

// punto 2
void generarOrdenDeCompra(const Usuario &usuario, vector<ItemCarrito> &carrito, int &contadorOrdenes,
                          vector<BonoUsuario> &bonos)
{
    if (carrito.empty())
    {
        cout << "\nEl carrito esta vacio. No se puede generar una orden de compra.\n";
        return;
    }

    OrdenDeCompra orden;
    orden.idOrden = ++contadorOrdenes;
    orden.usuario = usuario;
    orden.productos = carrito;
    orden.subtotal = 0.0;
    orden.descuentoBono = 0.0;

    // Calcular subtotal
    for (size_t i = 0; i < carrito.size(); ++i)
    {
        orden.subtotal += carrito[i].cantidad * carrito[i].producto.precio;
    }

    // punto 4c: verificar si el usuario tiene un bono activo sin usar
    double porcentajeBono = 0.0;
    for (size_t i = 0; i < bonos.size(); ++i)
    {
        if (bonos[i].idUsuario == usuario.id && !bonos[i].usado)
        {
            porcentajeBono = bonos[i].porcentajeDescuento;
            bonos[i].usado = true;
            break;
        }
    }

    // Aplicar descuento del bono al subtotal
    orden.descuentoBono = orden.subtotal * porcentajeBono;
    double subtotalConDescuento = orden.subtotal - orden.descuentoBono;

    // Calcular impuestos (19% IVA) y envio fijo
    orden.impuestos = subtotalConDescuento * 0.19;
    orden.envio = 5000.0;
    orden.total = subtotalConDescuento + orden.impuestos + orden.envio;

    // Guardar la orden en archivo .txt
    ofstream archivo("OrdenDeCompra.txt", ios::app);
    if (!archivo)
    {
        cout << "Error: No se pudo abrir o crear el archivo OrdenDeCompra.txt\n";
        return;
    }

    archivo << "=== ORDEN DE COMPRA #" << orden.idOrden << " ===\n";
    archivo << "Usuario: " << orden.usuario.nombre << " (ID: " << orden.usuario.id << ")\n";
    archivo << "Productos Comprados:\n";

    for (size_t i = 0; i < orden.productos.size(); ++i)
    {
        double subtotalProducto = orden.productos[i].cantidad * orden.productos[i].producto.precio;
        archivo << "  - " << orden.productos[i].producto.nombre
                << " | Cantidad: " << orden.productos[i].cantidad
                << " | Precio Unitario: $" << orden.productos[i].producto.precio
                << " | Subtotal: $" << subtotalProducto << "\n";
    }

    archivo << "-----------------------------------\n";
    archivo << "Subtotal:  $" << orden.subtotal << "\n";
    if (orden.descuentoBono > 0.0)
    {
        archivo << "Bono Descuento (50%): -$" << orden.descuentoBono << "\n";
        archivo << "Subtotal con Descuento: $" << subtotalConDescuento << "\n";
    }
    archivo << "Impuestos: $" << orden.impuestos << "\n";
    archivo << "Envio:     $" << orden.envio << "\n";
    archivo << "TOTAL:     $" << orden.total << "\n";
    archivo << "===================================\n\n";

    archivo.close();

    cout << "\n===================================\n";
    cout << "ORDEN DE COMPRA #" << orden.idOrden << " GENERADA CON EXITO!\n";
    if (orden.descuentoBono > 0.0)
    {
        cout << "Bono aplicado (50%): -$" << orden.descuentoBono << "\n";
    }
    cout << "Total a pagar: $" << orden.total << "\n";
    cout << "La orden ha sido guardada en 'OrdenDeCompra.txt'.\n";
    cout << "===================================\n";

    // Vaciar el carrito despues de la compra
    carrito.clear();
}
// punto 2

// =============================================
// PUNTO 4: Listar ganancias de la plataforma
// =============================================
void listarGanancias(vector<BonoUsuario> &bonos)
{
    ifstream archivo("OrdenDeCompra.txt");
    if (!archivo)
    {
        cout << "\nNo existe el archivo OrdenDeCompra.txt. No hay ordenes registradas.\n";
        return;
    }

    double totalSubtotales = 0.0;
    double totalImpuestos = 0.0;
    double totalGeneral = 0.0;
    int numOrdenes = 0;

    // Pedir al usuario el valor limite para el bono (punto 4b)
    double limiteParaBono;
    cout << "\nIngrese el valor limite para otorgar bono (ordenes que superen este total): $";
    cin >> limiteParaBono;
    cin.ignore();

    string linea;
    int idOrdenActual = -1;
    double subtotalActual = 0.0;
    double impuestosActual = 0.0;
    double totalActual = 0.0;
    int idUsuarioActual = -1;
    string nombreUsuarioActual = "";
    bool dentroDeOrden = false;

    cout << "\n=======================================================\n";
    cout << "         REPORTE DE GANANCIAS DE LA PLATAFORMA\n";
    cout << "=======================================================\n";

    while (getline(archivo, linea))
    {
        // Detectar inicio de orden
        if (linea.find("=== ORDEN DE COMPRA #") != string::npos)
        {
            dentroDeOrden = true;
            // Extraer id de orden
            stringstream ss(linea);
            string tmp;
            // Formato: "=== ORDEN DE COMPRA #N ==="
            size_t pos1 = linea.find('#');
            size_t pos2 = linea.find(' ', pos1);
            if (pos1 != string::npos && pos2 != string::npos)
            {
                string idStr = linea.substr(pos1 + 1, pos2 - pos1 - 1);
                idOrdenActual = convertirEntero(idStr);
            }
            subtotalActual = 0.0;
            impuestosActual = 0.0;
            totalActual = 0.0;
            idUsuarioActual = -1;
            nombreUsuarioActual = "";
        }

        // Extraer datos del usuario
        if (dentroDeOrden && linea.find("Usuario: ") != string::npos)
        {
            // Formato: "Usuario: NombreUsuario (ID: N)"
            size_t posNom = linea.find("Usuario: ") + 9;
            size_t posParenOpen = linea.find("(ID: ");
            if (posNom != string::npos && posParenOpen != string::npos)
            {
                nombreUsuarioActual = linea.substr(posNom, posParenOpen - posNom - 1);
                size_t posId = posParenOpen + 5;
                size_t posParenClose = linea.find(')', posId);
                if (posParenClose != string::npos)
                {
                    string idStr = linea.substr(posId, posParenClose - posId);
                    idUsuarioActual = convertirEntero(idStr);
                }
            }
        }

        // Extraer subtotal
        if (dentroDeOrden && linea.find("Subtotal:  $") != string::npos)
        {
            string val = linea.substr(linea.find('$') + 1);
            subtotalActual = convertirDouble(val);
        }

        // Extraer impuestos
        if (dentroDeOrden && linea.find("Impuestos: $") != string::npos)
        {
            string val = linea.substr(linea.find('$') + 1);
            impuestosActual = convertirDouble(val);
        }

        // Extraer total
        if (dentroDeOrden && linea.find("TOTAL:     $") != string::npos)
        {
            string val = linea.substr(linea.find('$') + 1);
            totalActual = convertirDouble(val);
        }

        // Detectar fin de orden
        if (dentroDeOrden && linea.find("===================================") != string::npos && linea.find("ORDEN") == string::npos)
        {

            if (idOrdenActual > 0 && totalActual > 0.0)
            {
                numOrdenes++;
                totalSubtotales += subtotalActual;
                totalImpuestos += impuestosActual;
                totalGeneral += totalActual;

                cout << "Orden #" << idOrdenActual
                     << " | Usuario: " << nombreUsuarioActual
                     << " | Subtotal: $" << subtotalActual
                     << " | Impuestos: $" << impuestosActual
                     << " | Total: $" << totalActual << "\n";

                // punto 4b: si el total supera el limite, generar bono 50%
                if (totalActual > limiteParaBono && idUsuarioActual > 0)
                {
                    // Verificar que no tenga ya un bono activo
                    bool yaTieneBono = false;
                    for (size_t i = 0; i < bonos.size(); ++i)
                    {
                        if (bonos[i].idUsuario == idUsuarioActual && !bonos[i].usado)
                        {
                            yaTieneBono = true;
                            break;
                        }
                    }
                    if (!yaTieneBono)
                    {
                        BonoUsuario nuevoBono;
                        nuevoBono.idUsuario = idUsuarioActual;
                        nuevoBono.porcentajeDescuento = 0.50;
                        nuevoBono.usado = false;
                        bonos.push_back(nuevoBono);
                        cout << "  *** BONO DEL 50% asignado a " << nombreUsuarioActual
                             << " por superar $" << limiteParaBono << " ***\n";
                    }
                }

                dentroDeOrden = false;
                idOrdenActual = -1;
            }
        }
    }

    archivo.close();

    if (numOrdenes == 0)
    {
        cout << "No hay ordenes de compra registradas.\n";
        return;
    }

    cout << "-------------------------------------------------------\n";
    cout << "TOTALES DE LA PLATAFORMA (" << numOrdenes << " ordenes):\n";
    cout << "  Subtotal total:   $" << totalSubtotales << "\n";
    cout << "  Impuestos totales: $" << totalImpuestos << "\n";
    cout << "  Total general:    $" << totalGeneral << "\n";
    cout << "=======================================================\n";

    // Mostrar bonos activos
    bool hayBonos = false;
    for (size_t i = 0; i < bonos.size(); ++i)
    {
        if (!bonos[i].usado)
        {
            if (!hayBonos)
            {
                cout << "\nBONOS ACTIVOS (disponibles para proximo carrito):\n";
                hayBonos = true;
            }
            cout << "  - Usuario ID " << bonos[i].idUsuario
                 << ": Descuento del " << (int)(bonos[i].porcentajeDescuento * 100) << "%\n";
        }
    }
    if (!hayBonos)
    {
        cout << "\nNo hay bonos activos actualmente.\n";
    }
}
// =============================================
int main()
{
    setlocale(LC_ALL, "spanish");
    vector<Usuario> usuarios = cargarUsuarios("Usuarios.txt");
    vector<Producto> productos = cargarProductos("Productos.txt");
    vector<ItemCarrito> carrito;
    vector<Comentario> listaComentarios = cargarComentarios("Comentarios.txt");

    // Leer el ultimo ID de orden para no duplicar IDs entre sesiones
    int contadorOrdenes = 0;
    {
        ifstream archivoOrd("OrdenDeCompra.txt");
        if (archivoOrd)
        {
            string ln;
            while (getline(archivoOrd, ln))
            {
                if (ln.find("=== ORDEN DE COMPRA #") != string::npos)
                {
                    size_t p1 = ln.find('#');
                    size_t p2 = ln.find(' ', p1);
                    if (p1 != string::npos && p2 != string::npos)
                    {
                        int id = convertirEntero(ln.substr(p1 + 1, p2 - p1 - 1));
                        if (id > contadorOrdenes)
                            contadorOrdenes = id;
                    }
                }
            }
        }
    }
    vector<BonoUsuario> bonos; // punto 4b: lista de bonos activos

    string correo, contrasena;
    cout << "Correo electr?nico: ";
    getline(cin, correo);
    cout << "Contrase?a: ";
    getline(cin, contrasena);

    Usuario *usuarioActual = iniciarSesion(usuarios, correo, contrasena);
    if (!usuarioActual)
    {
        cout << "Usuario o contrase?a inv?lidos.\n";
        return 0;
    }

    int opcion;
    do
    {
        cout << "\n=== MEN? PRINCIPAL ===\n";
        cout << "1. Listar productos con stock menor a 15\n";
        cout << "2. Listar todos los usuarios\n";
        cout << "3. Agregar producto al carrito\n";
        cout << "4. Ver carrito actual\n";
        cout << "5. Guardar carrito en archivo\n";
        cout << "6. Ver carrito guardado (archivo)\n";
        cout << "7. Visualizar comentarios\n";
        cout << "8. Listar productos del carrito por usuario\n";
        cout << "9. Pagar carrito y Generar Orden de Compra\n";
        cout << "10. Listar productos m?s vendidos\n";
        cout << "11. Listar ganancias de la plataforma\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opci?n: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion)
        {
        case 1:
        {
            vector<Producto> bajos = listarProductosBajoStock(productos);
            cout << "\n=== PRODUCTOS CON STOCK < 15 ===\n";
            for (size_t i = 0; i < bajos.size(); ++i)
                cout << bajos[i].id << " | " << bajos[i].nombre
                     << " | " << bajos[i].descripcion
                     << " | Precio: $" << bajos[i].precio
                     << " | Stock: " << bajos[i].stock << "\n";
            break;
        }
        case 2:
        {
            cout << "\n=== LISTA DE USUARIOS ===\n";
            for (size_t i = 0; i < usuarios.size(); ++i)
                cout << usuarios[i].id << " | " << usuarios[i].nombre
                     << " | " << usuarios[i].correo
                     << " | Direcci?n: " << usuarios[i].direccion
                     << " | Pago: " << usuarios[i].metodoPago << "\n";
            break;
        }
        case 3:
        {
            int idProd, cant;
            cout << "Ingrese el ID del producto: ";
            cin >> idProd;
            cout << "Cantidad: ";
            cin >> cant;
            cin.ignore();

            bool encontrado = false;
            for (size_t i = 0; i < productos.size(); ++i)
            {
                if (productos[i].id == idProd)
                {
                    if (productos[i].stock >= cant)
                    {
                        ItemCarrito item;
                        item.producto = productos[i];
                        item.cantidad = cant;

                        carrito.push_back(item);

                        productos[i].stock -= cant;

                        cout << "Producto agregado. Stock restante: " << productos[i].stock << "\n";
                    }
                    else
                    {
                        cout << "No hay suficiente stock. Disponible: " << productos[i].stock << "\n";
                    }
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado)
                cout << "Producto no encontrado.\n";
            break;
        }
        case 4:
            mostrarCarrito(carrito);
            break;
        case 5:
            guardarCarritoArchivo(*usuarioActual, carrito);
            cout << "Carrito guardado correctamente en Carrito.txt.\n";
            break;
        case 6:
            mostrarCarritoArchivo();
            break;
        case 7:
        {
            string fechaBusqueda;
            cout << "Ingrese la fecha l?mite (Formato AAAA-MM-DD, ej: 2025-06-01): ";
            getline(cin, fechaBusqueda);

            mostrarComentariosDesdeFecha(listaComentarios, fechaBusqueda);
            break;
        }

        case 8:
        {
            listarProductosCarritoPorUsuario();
            break;
        }

        case 9:
        {
            generarOrdenDeCompra(*usuarioActual, carrito, contadorOrdenes, bonos);
            break;
        }

        case 10:
        {
            listarProductosMasVendidos();
            break;
        }

        case 11:
        {
            listarGanancias(bonos);
            break;
        }

        case 0:
            cout << "Saliendo del sistema...\n";
            break;

        default:
            cout << "Opcion no valida.\n";
            break;
        }
    } while (opcion != 0);

    return 0;
}
