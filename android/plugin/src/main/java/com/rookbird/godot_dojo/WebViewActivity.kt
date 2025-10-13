package com.rookbird.godot_dojo

import android.annotation.SuppressLint
import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.webkit.JavascriptInterface
import android.webkit.WebResourceRequest
import android.webkit.WebResourceResponse
import android.webkit.WebView
import android.webkit.WebViewClient
import androidx.webkit.WebViewAssetLoader
import java.io.ByteArrayInputStream

class WebViewActivity : Activity() {

    companion object {
        const val EXTRA_URL = "extra_url"
        const val RESULT_PAYLOAD = "result_payload"
    }

    @SuppressLint("SetJavaScriptEnabled")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val webView = WebView(this)
        setContentView(webView)

        val url = intent.getStringExtra(EXTRA_URL)
        if (url == null) {
            finish()
            return
        }

        // Necesario para interceptar peticiones POST
        val assetLoader = WebViewAssetLoader.Builder()
            .addPathHandler("/assets/", WebViewAssetLoader.AssetsPathHandler(this))
            .build()

        webView.settings.javaScriptEnabled = true
        webView.addJavascriptInterface(WebAppInterface(this), "Android")

        webView.webViewClient = object : WebViewClient() {
            // Interceptamos todas las peticiones
            override fun shouldInterceptRequest(
                view: WebView,
                request: WebResourceRequest
            ): WebResourceResponse? {
                // Buscamos la petición POST al callback de localhost
                if (request.method == "POST" && request.url.host == "localhost" && request.url.path == "/callback") {
                    Log.d("WebViewActivity", "Intercepting POST to /callback")
                    // Inyectamos un script que captura el payload y lo envía a nuestro puente de JS
                    val script = """
                        <script>
                        fetch('${request.url}', {
                            method: 'POST',
                            headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                            body: document.body.innerText
                        })
                        .then(response => response.text())
                        .then(data => {
                             // El 'data' aquí es el payload que queremos.
                             // Lo pasamos a la interfaz de Kotlin.
                             Android.processPayload(document.body.innerText);
                        })
                        .catch(error => console.error('Error:', error));
                        </script>
                    """
                    // Devolvemos una respuesta vacía para que el navegador no intente realmente ir a localhost
                    return WebResourceResponse("text/html", "UTF-8", ByteArrayInputStream(script.toByteArray()))
                }
                // For all other requests, let the WebViewAssetLoader handle them.
                return assetLoader.shouldInterceptRequest(request.url)
            }
        }

        webView.loadUrl(url)
    }

    // Esta clase es el puente entre JavaScript y Kotlin
    inner class WebAppInterface(private val activity: Activity) {
        @JavascriptInterface
        fun processPayload(payload: String) {
            Log.d("WebViewActivity", "Payload received from JS: $payload")
            val resultIntent = Intent()
            resultIntent.putExtra(RESULT_PAYLOAD, payload)
            activity.setResult(Activity.RESULT_OK, resultIntent)
            activity.finish() // Cerramos el WebView
        }
    }
}