var express = require('express');
var connect = require('connect');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var fs = require('fs');
var http = require('http');
var multer = require('multer');
var multiparty = require('multiparty');
var _storage = multer.diskStorage({
	destination:function (req,file,cb){
	cb(null, __dirname +'/pic-colo/uploads/');
	},
	filename:function(req,file,cb){
	cb(null, 'GetImage');
	}
	})
var upload = multer({ storage : _storage});

var index = require('./routes/index');
var users = require('./routes/users');

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');
// port setup
app.set('port', process.env.PORT || 9000);

// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static('pic-colo'));

app.get('/', function (req,res){
	fs.readFile('./pic-colo/index.html', function (error, data){
	res.writeHead(200, { 'Content-Type': 'text/html'});
	res.end(data);
	});
});
app.post('/upload',upload.single('photo'), function(req,res){
	console.log(req.body);
	console.log(req.file);
	res.status(204).end();

});
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

///////////////////////////////////////
var server = app.listen(app.get('port'), function() {
	console.log('Express server linstening on port ' + server.address().port);
});
