const express = require('express');
const bodyParser = require('body-parser');
const mysql = require('mysql2');
const crypto = require('crypto');
const cors = require('cors');
const app = express();
const PORT = 5000;
app.use(bodyParser.json());
app.use(cors());

const connection = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '',
  database: 'pdeu_project'
});
const generateHashId = () => {
  return crypto.createHash('md5').update(Math.random().toString()).digest('hex');
};
const formatTime = (dateObj) => {
  let hours = dateObj.getHours();
  let minutes = dateObj.getMinutes();
  const ampm = hours >= 12 ? 'pm' : 'am';
  hours = hours % 12;
  hours = hours ? hours : 12; // 0 becomes 12
  minutes = minutes < 10 ? '0' + minutes : minutes;
  return `${hours}:${minutes} ${ampm}`;
};

const formatDate = (dateObj) => {
  const day = String(dateObj.getDate()).padStart(2, '0');
  const month = String(dateObj.getMonth() + 1).padStart(2, '0');
  const year = dateObj.getFullYear();
  return `${day}-${month}-${year}`;
};

app.post('/post/sensor-data', (req, res) => {
    const { soil_value } = req.body;

    if (soil_value !== undefined) {
        console.log(`Received data - Soil Moisture: ${soil_value}`);

        const dateObj = new Date();
        const time = formatTime(dateObj); 
        const date = formatDate(dateObj); 
        const hashid = generateHashId();
        const query = 'INSERT INTO data (hashid, soil_value, time, date) VALUES (?, ?, ?, ?)';
        connection.execute(query, [hashid, soil_value, time, date], (err, results) => {
            if (err) {
                console.error('Error inserting data into database:', err);
                return res.status(500).json({
                    status: 'error',
                    message: 'Failed to insert data into the database.',
                });
            }

            console.log('Data inserted successfully into database:', results);

            res.status(200).json({
                status: 'success',
                message: 'Data received and inserted successfully',
                data: {
                    soil_value,
                },
            });
        });
    } else {
        console.error('Invalid data received');
        res.status(400).json({
            status: 'error',
            message: 'Invalid data. Soil Moisture Values are required.',
        });
    }
});
app.get('/post/get-data', (req, res) => {
    const query = 'SELECT * FROM data order by id desc';

    connection.execute(query, (err, results) => {
        if (err) {
            console.error('Error retrieving data from database:', err);
            return res.status(500).json({
                status: 'error',
                message: 'Failed to retrieve data from the database.',
            });
        }

        console.log('Retrieved data successfully:', results);

        res.status(200).json({
            data: results,  
        });
    });
});

app.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});
