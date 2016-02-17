
document.addEventListener('DOMContentLoaded', function(event) {

  var do_debug = true;
  var gl = null;

  function throwOnGLError(err, funcName, args) {
    throw WebGLDebugUtils.glEnumToString(err) + " was caused by call to: " + funcName;
  }

  function initWebGL(canvas) {
  
    try {
      // Try to grab the standard context. If it fails, fallback to experimental.
      gl = canvas.getContext("webgl") || canvas.getContext("experimental-webgl");
      gl.viewportWidth = canvas.width;
      gl.viewportHeight = canvas.height;
    }
    catch(e) {}
  
    // If we don't have a GL context, give up now
    if (!gl)
      alert("Unable to initialize WebGL. Your browser may not support it.");
      
    if (do_debug)
      gl = WebGLDebugUtils.makeDebugContext(gl, throwOnGLError);
      
  }
  
  function getShader(gl, id) {
    var shaderScript, theSource, currentChild, shader;
  
    shaderScript = document.getElementById(id);
  
    if (!shaderScript)
      return null;
  
    theSource = "";
    currentChild = shaderScript.firstChild;
  
    while(currentChild) {
      if (currentChild.nodeType == currentChild.TEXT_NODE)
        theSource += currentChild.textContent;
    
      currentChild = currentChild.nextSibling;
    }
    
    if (shaderScript.type == "x-shader/x-fragment")
      shader = gl.createShader(gl.FRAGMENT_SHADER);
    else if (shaderScript.type == "x-shader/x-vertex")
      shader = gl.createShader(gl.VERTEX_SHADER);
    else
     return null;
     
    gl.shaderSource(shader, theSource);
    
    // Compile the shader program
    gl.compileShader(shader);
    
    // See if it compiled successfully
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
      alert("An error occurred compiling the shaders: " + gl.getShaderInfoLog(shader));
      return null;
    }
    
    return shader;
  }
  
  var vertexPositionAttribute;
  var vertexNormalAttribute;
  var shaderProgram;
  var indexBuffer;
  var xmin, ymin, zmin;
  var xmax, ymax, zmax;
  var scale;
  var cur_rad = 0;
  var timeout;
  var mod;
  
  function initShaders(gl) {
    var fragmentShader = getShader(gl, "shader-fs");
    var vertexShader = getShader(gl, "shader-vs");
  
    // Create the shader program
  
    shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);
  
    // If creating the shader program failed, alert
  
    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS))
      alert("Unable to initialize the shader program.");
      
    gl.useProgram(shaderProgram);
  
    vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    gl.enableVertexAttribArray(vertexPositionAttribute);
    vertexNormalAttribute = gl.getAttribLocation(shaderProgram, "aVertexNormal");
    gl.enableVertexAttribArray(vertexNormalAttribute);
  }
  
  function initBuffers() {
  
    try {
      for (var mi in mod.meshes) {
        var m = mod.meshes[mi];
        m.vertices_buffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, m.vertices_buffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(m.vertices), gl.STATIC_DRAW);
        m.normals_buffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, m.normals_buffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(m.normals), gl.STATIC_DRAW);
        
        var faces = [];
        for (var fi in m.faces) {
          var f = m.faces[fi];
          faces.push(f[0], f[1], f[2]);
        }
        m.index_buffer = gl.createBuffer();
        m.index_buffer_length = faces.length;
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, m.index_buffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(faces), gl.STATIC_DRAW);
      }
      
    } catch (err) {
      console.error(err);
    }

  }

  function setMatrixUniforms(gl, perspectiveMatrix, mvMatrix) {
    var pUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
    gl.uniformMatrix4fv(pUniform, false, new Float32Array(perspectiveMatrix));

    var mvUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
    gl.uniformMatrix4fv(mvUniform, false, new Float32Array(mvMatrix));
  }
  
  function drawScene() {

    gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    
    var perspectiveMatrix = mat4.create();
    mat4.perspective(45, gl.viewportWidth / gl.viewportHeight, 0.1, 100.0, perspectiveMatrix);
    var mvMatrix = mat4.create();
    mat4.identity(mvMatrix);
    mat4.scale(mvMatrix, [scale, scale, scale]);
    mat4.rotate(mvMatrix, cur_rad, [0, 1, 0], mvMatrix);
    mat4.rotate(mvMatrix, cur_rad/1.5, [1, 0, 0], mvMatrix);
    cur_rad += .02;
    mvMatrix[14] = -3;
    setMatrixUniforms(gl, perspectiveMatrix, mvMatrix);
    
    for (var mi in mod.meshes) {
      var m = mod.meshes[mi];
      
      gl.bindBuffer(gl.ARRAY_BUFFER, m.vertices_buffer);
      gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
      
      gl.bindBuffer(gl.ARRAY_BUFFER, m.normals_buffer);
      gl.vertexAttribPointer(vertexNormalAttribute, 3, gl.FLOAT, false, 0, 0);
      
      gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, m.index_buffer);
      gl.drawElements(gl.TRIANGLES, m.index_buffer_length, gl.UNSIGNED_SHORT, 0);
    }
    timeout = setTimeout(function() {drawScene();}, 10);
    
  }
  
  function start_render(model_name) {

    importAsset(model_name).then(function(result) {
    
      var new_mod;
      try {
        new_mod = JSON.parse(result);
      }
      catch (err) {
        console.error('json parse failed, trying:');
        console.log(result);
        return;
      }
    
      if (timeout)
        clearTimeout(timeout);
      if (mod) {
        for (var mi in mod.meshes) {
          var m = mod.meshes[mi];
          gl.deleteBuffer(m.vertices_buffer);
          gl.deleteBuffer(m.normals_buffer);
          gl.deleteBuffer(m.index_buffer);
        }
      }
      
      mod = new_mod;
      
      xmin = ymin = zmin = 100000;
      xmax = ymax = zmax = -100000;
      for (var mi in mod.meshes) {
        var m = mod.meshes[mi];
        for (var vi = 0; vi < m.vertices.length; ++vi) {
          var v = m.vertices[vi];
          if (v < xmin)
            xmin = v;
          if (v > xmax)
            xmax = v;
          v = m.vertices[++vi];
          if (v < ymin)
            ymin = v;
          if (v > ymax)
            ymax = v;
          v = m.vertices[++vi];
          if (v < zmin)
            zmin = v;
          if (v > zmax)
            zmax = v;
        }
      }
      var max_scale = xmax;
      if (-xmin > max_scale)
        max_scale = -xmin;
      if (ymax > max_scale)
        max_scale = ymax;
      if (-ymin > max_scale)
        max_scale = -ymin;
      if (zmax > max_scale)
        max_scale = zmax;
      if (-zmin > max_scale)
        max_scale = -zmin;
      scale = .7/max_scale;

      initBuffers();
      drawScene();
    },
    function(msg) {
      console.error(msg);
    });
  }

  var file_promises = [];
  var file_names = [];
  var num_fs_entries = 0;
  
  function done_with_files() {
  
    Promise.all(file_promises).then(function() {
      if (!gl) {
        var canvas = document.getElementById("glcanvas");
        initWebGL(canvas);
        
        if (gl) {
          gl.clearColor(0.0, 0.0, 0.0, 1.0);
          gl.clearDepth(1.0);
          gl.enable(gl.DEPTH_TEST);
          gl.depthFunc(gl.LEQUAL);
          initShaders(gl);
        }
      }
      if (file_names.length === 1)
        start_render(file_names[0]);
    },function() {
      console.error('file system transfer failed');
    });
  }
  
  function add_file(f, file_name) {
    var reader = new FileReader();
    reader.onload = function (e) {
      file_names.push(file_name);
      file_promises.push(addToFileSystem(file_name, false, reader.result));
      if (file_promises.length === num_fs_entries)
        done_with_files();
    };
    reader.onerror = function (e) {
      console.error(' error: ' + e);
    };
    reader.readAsArrayBuffer(f);
  }
  
  function add_fs_entry(entry) {
    ++num_fs_entries;
    if (entry instanceof File)
      add_file(entry, entry.name);
    else if (entry.isFile) {
      entry.file(function(f) {
        add_file(f, entry.fullPath);
      });
    } else
      file_promises.push(addToFileSystem(entry.fullPath, true));
  }
  
  function add_fs_entries(items) {
    for (var i = 0, item; item = items[i]; i++) {
      var entry = item;
      try {
        if (entry instanceof DataTransferItem)
          entry = entry.webkitGetAsEntry();
      }
      catch(err)
      {}
      add_fs_entry(entry);
      if (entry.isDirectory) {
        ++num_fs_entries;
        var reader = entry.createReader();
        reader.readEntries(function(entries) {
          --num_fs_entries;
          add_fs_entries(entries);
        });
      }
    }
    if (file_promises.length === num_fs_entries)
      done_with_files();
  }

  function handleDrop(e) {
    e.preventDefault();
    
    clearFileSystem();
    
    file_promises = [];
    file_names = [];
    num_fs_entries = 0;
    
    var files;
    if (e.target && e.target.files)
      files = e.target.files;
    else if (e.dataTransfer)
      files = e.dataTransfer.items || e.dataTransfer.files;
    if (files)
      add_fs_entries(files);
  }

  function allowDrop(e) {
    e.preventDefault();
  }

  var canvas = document.getElementById("glcanvas");
  canvas.ondrop = handleDrop;
  canvas.ondragover = allowDrop;
  
  loadAssimpLib();

  if (!gl) {
    var canvas = document.getElementById("glcanvas");
    initWebGL(canvas);
    
    if (gl) {
      gl.clearColor(0.0, 0.0, 0.0, 1.0);
      gl.clearDepth(1.0);
      gl.enable(gl.DEPTH_TEST);
      gl.depthFunc(gl.LEQUAL);
      initShaders(gl);
    }
  }
  start_render("/resources/WusonOBJ.obj");
  
});



