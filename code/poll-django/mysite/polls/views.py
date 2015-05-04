from django.shortcuts import render, get_object_or_404
# get_list_or_404处理多个对象时使用
from django.http import HttpResponse
from polls.models import Poll
from django.template import Context, loader

def index(request):
    latest_poll_list = Poll.objects.order_by('-pub_date')[:5]
    # 这一句不知道啥用的
    # template = loader.get_template('polls/index.html')
    context = {'latest_poll_list': latest_poll_list}
    return render(request, 'polls/index.html', context)

def detail(request, poll_id):
    poll = get_object_or_404(Poll, pk=poll_id)
    return render(request, 'polls/detail.html', {'poll': poll})

def results(request, poll_id):
    poll = get_object_or_404(Poll, pk=poll_id)
    context = {'poll': poll}
    return render(request, 'polls/results.html', context)

def vote(request, poll_id):
    poll = get_object_or_404(Poll, pk=poll_id)
    context = {'poll' : poll}
    return render(request, 'polls/vote.html', context)

# Create your views here.
