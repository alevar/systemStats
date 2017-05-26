#initializes the App
from flask import Flask, request, Blueprint, render_template, \
                  flash, g, session, redirect, url_for

app = Flask(__name__)
app.config.from_object('ServerLog.flaskconfig')

@app.errorhandler(404)
def not_found(error):
    return "404 error", 404

app.url_map.strict_slashes = False
import ServerLog.controllers
