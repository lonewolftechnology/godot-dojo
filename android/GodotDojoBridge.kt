package org.godotengine.godot

import android.content.ActivityNotFoundException
import android.net.Uri
import android.util.Log
import androidx.browser.customtabs.CustomTabsIntent
import org.godotengine.godot.plugin.GodotPlugin
import org.godotengine.godot.plugin.UsedByGodot

class GodotDojoBridge(godot: Godot) : GodotPlugin(godot) {

    companion object {
        // Usar un TAG para los logs es una buena práctica en Android.
        val TAG = GodotDojoBridge::class.java.simpleName
    }

    override fun getPluginName() = "GodotDojoBridge"

    /**
     * Este método es llamado desde C++ a través de `object->call()`.
     * Recibe una URL y la abre en una Chrome Custom Tab.
     */
    @UsedByGodot
    fun openCustomTab(url: String) {
        // `activity` es una propiedad del GodotPlugin que nos da acceso a la actividad principal.
        // Es importante ejecutar el código de UI en el hilo principal de UI.
        // Usamos `activity?.let` para un manejo más seguro y idiomático de nulos en Kotlin.
        activity?.let { currentActivity ->
            currentActivity.runOnUiThread {
            try {
                val customTabsIntent = CustomTabsIntent.Builder()
                    .setShowTitle(true)
                    // Aquí puedes personalizar colores para que coincida con tu app
                    // .setToolbarColor(ContextCompat.getColor(currentActivity, R.color.your_color))
                    .build()
                customTabsIntent.launchUrl(currentActivity, Uri.parse(url))
            } catch (e: ActivityNotFoundException) {
                Log.e(TAG, "No se encontró un navegador compatible con Custom Tabs. Error: ${e.message}")
                // Opcional: podrías emitir una señal de vuelta a Godot para notificar el error.
            } catch (e: SecurityException) {
                Log.e(TAG, "Error de seguridad al intentar abrir la Custom Tab. Revisa los permisos. Error: ${e.message}")
            } catch (e: Exception) {
                // Manejar error, por ejemplo, si Chrome no está instalado o hay otro problema.
                Log.e(TAG, "Error inesperado al abrir la Custom Tab: ${e.message}")
            }
            }
        }
    }
}