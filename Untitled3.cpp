#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <clocale>
using namespace std;

struct Usuario {
    int id;
    string nombre;
    string correo;
    string contrasena;
    string direccion;
    string metodoPago;
};

struct Producto {
    int id;
    string nombre;
    string descripcion;
    double precio;
    int stock;
};

struct ItemCarrito {
    Producto producto;
    int cantidad;
};

struct Comentario {
    int id;
    string producto;  
    string usuario;    
    string texto;      
    string fecha;     
};


int convertirEntero(const string &texto) {
    int valor = 0;
    stringstream ss(texto);
    ss >> valor;
    return valor;
}

double convertirDouble(const string &texto) {
    double valor = 0;
    stringstream ss(texto);
    ss >> valor;
    return valor;
}


vector<Usuario> cargarUsuarios(const string &nombreArchivo) {
    vector<Usuario> usuarios;
    ifstream archivo(nombreArchivo.c_str());
    string linea;
    getline(archivo, linea); 

    while (getline(archivo, linea)) {
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

vector<Producto> cargarProductos(const string &nombreArchivo) {
    vector<Producto> productos;
    ifstream archivo(nombreArchivo.c_str());
    string linea;
    getline(archivo, linea); 

    while (getline(archivo, linea)) {
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

vector<Comentario> cargarComentarios(const string &nombreArchivo) {
    vector<Comentario> comentarios;
    ifstream archivo(nombreArchivo.c_str());
    string linea;

    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir " << nombreArchivo << endl;
        return comentarios;
    }

  
    getline(archivo, linea); 

    while (getline(archivo, linea)) {
        if (linea.empty()) continue; 

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


Usuario *iniciarSesion(vector<Usuario> &usuarios, const string &correo, const string &contrasena) {
    for (size_t i = 0; i < usuarios.size(); ++i)
        if (usuarios[i].correo == correo && usuarios[i].contrasena == contrasena)
            return &usuarios[i];
    return NULL;
}


vector<Producto> listarProductosBajoStock(const vector<Producto> &productos) {
    vector<Producto> bajos;
    for (size_t i = 0; i < productos.size(); ++i)
        if (productos[i].stock < 15)
            bajos.push_back(productos[i]);
    return bajos;
}


void guardarCarritoArchivo(const Usuario &usuario, const vector<ItemCarrito> &carrito) {
    ofstream archivo("Carrito.txt", ios::app); 
    for (size_t i = 0; i < carrito.size(); ++i) {
        double subtotal = carrito[i].cantidad * carrito[i].producto.precio;
        archivo << usuario.id << ","
                << usuario.nombre << ","
                << carrito[i].producto.id << ","
                << carrito[i].producto.nombre << ","
                << carrito[i].cantidad << ","
                << carrito[i].producto.precio << ","
                << subtotal << "\n";
    }
    archivo.close();
}

void mostrarCarritoArchivo() {
    ifstream archivo("Carrito.txt");
    if (!archivo) {
        cout << "No hay carrito registrado.\n";
        return;
    }

    string linea;
    cout << "\n=== CONTENIDO DEL CARRITO GUARDADO ===\n";
    cout << "Usuario | Producto | Cantidad | Precio | Subtotal\n";
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string idUsuario, nombreUsuario, idProd, nombreProd, cantidad, precio, subtotal;
        getline(ss, idUsuario, ',');
        getline(ss, nombreUsuario, ',');
        getline(ss, idProd, ',');
        getline(ss, nombreProd, ',');
        getline(ss, cantidad, ',');
        getline(ss, precio, ',');
        getline(ss, subtotal, ',');
        cout << nombreUsuario << " | " << nombreProd << " | " << cantidad
             << " | $" << precio << " | $" << subtotal << "\n";
    }
    archivo.close();
}

void mostrarCarrito(const vector<ItemCarrito> &carrito) {
    cout << "\n=== CARRITO DE COMPRAS ===\n";
    double total = 0;
    for (size_t i = 0; i < carrito.size(); ++i) {
        double subtotal = carrito[i].cantidad * carrito[i].producto.precio;
        cout << carrito[i].producto.nombre
             << " (Precio: $" << carrito[i].producto.precio
             << ", Cantidad: " << carrito[i].cantidad
             << ") = $" << subtotal << "\n";
        total += subtotal;
    }
    cout << "TOTAL: $" << total << "\n";
}

void mostrarComentariosDesdeFecha(const vector<Comentario> &comentarios, const string &fechaLimite) {
    cout << "\n=== COMENTARIOS DESDE EL " << fechaLimite << " EN ADELANTE ===\n";
    bool encontrado = false;

    for (size_t i = 0; i < comentarios.size(); ++i) {
        if (comentarios[i].fecha >= fechaLimite) {
            cout << "[" << comentarios[i].fecha << "] " 
                 << "Producto: " << comentarios[i].producto << " | "
                 << "Usuario: " << comentarios[i].usuario << "\n"
                 << "Comentario: " << comentarios[i].texto << "\n";
            cout << "------------------------------------------\n";
            encontrado = true;
        }
    }

    if (!encontrado) {
        cout << "No se encontraron comentarios despu�s de esa fecha.\n";
    }
}


int main() {
	setlocale(LC_ALL, "spanish");
    vector<Usuario> usuarios = cargarUsuarios("Usuarios.txt");
    vector<Producto> productos = cargarProductos("Productos.txt");
    vector<ItemCarrito> carrito;
    vector<Comentario> listaComentarios = cargarComentarios("Comentarios.txt");

    string correo, contrasena;
    cout << "Correo electr�nico: ";
    getline(cin, correo);
    cout << "Contrase�a: ";
    getline(cin, contrasena);

    Usuario *usuarioActual = iniciarSesion(usuarios, correo, contrasena);
    if (!usuarioActual) {
        cout << "Usuario o contrase�a inv�lidos.\n";
        return 0;
    }

    int opcion;
    do {
        cout << "\n=== MEN� PRINCIPAL ===\n";
        cout << "1. Listar productos con stock menor a 15\n";
        cout << "2. Listar todos los usuarios\n";
        cout << "3. Agregar producto al carrito\n";
        cout << "4. Ver carrito actual\n";
        cout << "5. Guardar carrito en archivo\n";
        cout << "6. Ver carrito guardado (archivo)\n";
        cout << "7. Visualizar comentarios\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opci�n: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        case 1: {
            vector<Producto> bajos = listarProductosBajoStock(productos);
            cout << "\n=== PRODUCTOS CON STOCK < 15 ===\n";
            for (size_t i = 0; i < bajos.size(); ++i)
                cout << bajos[i].id << " | " << bajos[i].nombre
                     << " | " << bajos[i].descripcion
                     << " | Precio: $" << bajos[i].precio
                     << " | Stock: " << bajos[i].stock << "\n";
            break;
        }
        case 2: {
            cout << "\n=== LISTA DE USUARIOS ===\n";
            for (size_t i = 0; i < usuarios.size(); ++i)
                cout << usuarios[i].id << " | " << usuarios[i].nombre
                     << " | " << usuarios[i].correo
                     << " | Direcci�n: " << usuarios[i].direccion
                     << " | Pago: " << usuarios[i].metodoPago << "\n";
            break;
        }
       case 3: {
            int idProd, cant;
            cout << "Ingrese el ID del producto: ";
            cin >> idProd;
            cout << "Cantidad: ";
            cin >> cant;
            cin.ignore();

            bool encontrado = false;
            for (size_t i = 0; i < productos.size(); ++i) {
                if (productos[i].id == idProd) {
                    if (productos[i].stock >= cant) {
                        ItemCarrito item;
                        item.producto = productos[i];
                        item.cantidad = cant;
                        
                        carrito.push_back(item);
                        
                        productos[i].stock -= cant; 
                        
                        cout << "Producto agregado. Stock restante: " << productos[i].stock << "\n";
                    } else {
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
        case 7: {
    string fechaBusqueda;
    cout << "Ingrese la fecha l�mite (Formato AAAA-MM-DD, ej: 2025-06-01): ";
    getline(cin, fechaBusqueda);
    
    mostrarComentariosDesdeFecha(listaComentarios, fechaBusqueda);
    break;
}
        case 0:
            cout << "Saliendo del sistema...\n";
            break;
        default:
            cout << "Opci�n no v�lida.\n";
        }
    } while (opcion != 0);

    return 0;
}
