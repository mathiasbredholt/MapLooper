var HtmlWebpackPlugin = require('html-webpack-plugin');
var HtmlWebpackInlineSourcePlugin =
    require('html-webpack-inline-source-plugin');
const path = require('path');

module.exports = {
  entry: './src/index.js',
  output: {
    filename: 'main.js',
    path: path.resolve(__dirname, 'dist'),
  },
  mode: 'production',
  plugins: [new HtmlWebpackPlugin({
    inlineSource: '.(js|css)$'
  }), new HtmlWebpackInlineSourcePlugin()]
};
