var fs = require('fs');

module.exports = function(app) {

app.get('/',function(req,res){
	res.end("Node=File-upload");
});
app.post('/upload', function(req,res) {
	console.log(req.files.image.originalFilename);
	console.log(req.files.image.path);
		fs.readFile(req.files.image.path, function (err, data){
		var dirname = "/home/ec2-user/Node/file-upload";
		var newPath = dirname + "/uploads/" + req.files.image.originalFilename;
		fs.writeFile(newPath, data, function (err) {
		if(err){
		res.json({'respones':"Error"});
		}else {
		res.json({'response':"Saved"});
}
});
});
});

app.get('/uploads/:file', function (req, res){
		file = req.params.file;
		var dirname = "/home/ec2-user/Node/file-upload";
		var img = fs.readFileSync(dirname + "/uploads/" + file);
		res.writeHead(200, {'Content-Type': 'image/jpg' });
		res.end(img, 'binary');

});
};
