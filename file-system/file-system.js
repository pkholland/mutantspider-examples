
document.addEventListener('DOMContentLoaded', function(event) {

  var elem = document.getElementById('test_output');
  filesys_watch(function(msg) {
    elem.innerHTML += msg;
  });
  
});
