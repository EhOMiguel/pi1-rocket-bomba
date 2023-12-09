from pymongo import MongoClient
import pandas as pd
import plotly.graph_objects as go

client = MongoClient("mongodb+srv://pitao:1234@bombapi1.c6frnsa.mongodb.net/?retryWrites=true&w=majority")
try:
    databases = client.list_database_names()
    print("Sucesso na conexão com o mongoDB")
except Exception as e:
    print("Erro ao conectar ao MongoDB:", e)

db = client.primeiroLancamento
collection = db.lancamentos


lançamentos = collection.find({}) 
window_size = 5

for lançamento in lançamentos:
    df = pd.DataFrame(lançamento['passeio'])

    # Ajustar a altitude para começar de 0
    altitude_inicial = df['altitude'].iloc[0]
    df['altitude'] = df['altitude'] - altitude_inicial

    # Calcular as médias móveis
    df['latitude_movel'] = df['latitude'].rolling(window=window_size).mean()
    df['longitude_movel'] = df['longitude'].rolling(window=window_size).mean()
    df['altitude_movel'] = df['altitude'].rolling(window=window_size).mean()

    
    fig = go.Figure(data=[go.Scatter3d(
        x=df['longitude_movel'],
        y=df['latitude_movel'],
        z=df['altitude_movel'],
        mode='lines+markers',
        marker=dict(
            size=3,
            color=df['velocidadeInst'], 
            colorscale='bluered', 
            opacity=1.0
        ),
        line=dict(
            color='blue', 
            width=2       
        )
    )])


    fig.update_layout(
        title=f'Trajetória do Lançamento {lançamento["idLancamento"]} (Média Móvel)',
        scene=dict(
            xaxis_title='Longitude',
            yaxis_title='Latitude',
            zaxis_title='Altitude'
        )
    )

    fig.show()
