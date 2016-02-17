# web3D

Mutantspider example that compiles the [Open Asset Import Library](https://github.com/assimp/assimp).  To build this example:
```
cd <wherever>/mutantspider-examples/web3d
npm install
make run_server
```
Then point your browser to localhost:5103.  This shows a simple web page.  Drag and drop a 3D file onto the web page and it will (or should) parse the file using the Asset Import library, and do a simple 3D display of the model.  A pre-built version
of this application can be found [here](http://www.mutantspider.tech/code/web3d/index.html).
